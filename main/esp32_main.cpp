// main.cpp - ESP-IDF, FreeRTOS-based refactor

#include <memory>
#include <string>
#include <chrono>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "timeUtil.h"          // getUnixTime, toIsoTimestampLocal, timeSetup
#include "webConnectivity.h"   // httpsGet
#include "logger.h"
#include "common.h"

#include "icalHandler.h"
#include "icalEventHandlers/relayHandler.h"
#include "icalEventHandlers/relayHandlerMaxTimed.h"
// #include "icalEventHandlers/fastLedHandler.h"
#include "icalEventHandlers/lambdaHandler.h"

// #include "ota.h"
#include "printers/printerCollection.h"
#if TELNET_PORT != 0
#include "printers/telnetServer.h"
#endif

#include "config.h" // WIFI_SSID, WIFI_PASSWORD, etc.

static IcalHandler* icalHandler = nullptr;
static SemaphoreHandle_t calendar_mutex = nullptr; // protects access to icalHandler pointer

// Task priorities / stack sizes (tune as needed)
constexpr int OTA_TASK_PRIO = 5;
constexpr int CALENDAR_TASK_PRIO = 4;
constexpr int EVENT_TASK_PRIO = 3;
constexpr size_t OTA_TASK_STACK = 4 * 1024;
constexpr size_t CALENDAR_TASK_STACK = 8 * 1024;
constexpr size_t EVENT_TASK_STACK = 8 * 1024;

// Forward declarations
void setup_initial_handlers(IcalHandler* handler);

// ------------------ Calendar download & refresh task --------------------

static void calendarTask(void* arg) {
    Logger::getInstance().info("Calendar task starting");

    while (true) {
        // Download calendar
        {
            Logger::getInstance().info("Fetching calendar");
            IcalIterator_ptr newIterator;

            // Download + wait for time sync
            time_t now = getUnixTime();
            while (now == 0) {
                Logger::getInstance().info("Waiting for time sync before calendar download");
                vTaskDelay(pdMS_TO_TICKS(500));
                now = getUnixTime();
            }

            std::string ical_str = httpsGet(ICAL_URL, ROOT_CA_CERTIFICATE);
            newIterator = new_shared_ptr<IcalIterator>(ical_str, now);

            // Swap handler under lock
            if (xSemaphoreTake(calendar_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
                delete icalHandler; // assuming owning; adjust if using smart pointers
                icalHandler = new IcalHandler(newIterator);
                setup_initial_handlers(icalHandler);
                xSemaphoreGive(calendar_mutex);
                Logger::getInstance().info("Calendar handler updated");
            }
            else {
                Logger::getInstance().warn("Failed to acquire calendar mutex to update handler");
            }
        }

        // TODO: implement proper backoff / respect caching headers or ICS refresh interval
        constexpr TickType_t delay_between_downloads = pdMS_TO_TICKS(5 * 60 * 1000); // every 5 minutes
        vTaskDelay(delay_between_downloads);
    }

    // never returns
}

// ------------------ Event processing task --------------------

static void eventProcessingTask(void* arg) {
    Logger::getInstance().info("Event processing task starting");

    while (true) {
        time_t current_time = getUnixTime();

        if (xSemaphoreTake(calendar_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
            if (icalHandler) {
                icalHandler->processEventsUntil(current_time);

                time_t next_event_time = icalHandler->getTimeOfNextEvent();
                time_t wait_time = next_event_time - current_time;

                Logger::getInstance().info("Current time: ", toIsoTimestampLocal(current_time));
                Logger::getInstance().info("Next event at: ", toIsoTimestampLocal(next_event_time));
                Logger::getInstance().info("Sleeping until next event in seconds: ", wait_time);

                // print queue for visibility
                icalHandler->printEventQueue();

                xSemaphoreGive(calendar_mutex);

                // If next event is far in future, sleep here; otherwise spin more granularly
                if (wait_time > 2) {
                    // sleep until just before event
                    TickType_t sleep_ticks = pdMS_TO_TICKS((wait_time - 1) * 1000);
                    vTaskDelay(sleep_ticks);
                }
                else {
                    // small delay to avoid tight spin
                    vTaskDelay(pdMS_TO_TICKS(500));
                }
            }
            else {
                xSemaphoreGive(calendar_mutex);
                Logger::getInstance().warn("icalHandler not yet initialized");
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
        else {
            Logger::getInstance().warn("Failed to acquire calendar mutex in event task");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

// ------------------ OTA task --------------------

static void otaTask(void* arg) {
    Logger::getInstance().info("OTA task starting");
    // otaSetup(OTA_PORT, ESP_NETWORK_HOSTNAME, OTA_PASSWORD);
    while (true) {
        // otaLoop(); // non-blocking in your design

        // TODO: if you want to expose periodic health/logging or trigger OTA retry logic, do it here

        vTaskDelay(pdMS_TO_TICKS(500)); // run twice a second
    }
}

// ------------------ Helper to add handlers --------------------

void setup_initial_handlers(IcalHandler* handler) {
    if (!handler) return;

    // Top light
    handler->registerEventHandler(new RelayHandler("Light - Bright", GPIO_NUM_32, true));

    // CO2
    handler->registerEventHandler(new RelayHandler("CO2", GPIO_NUM_33, true));

    // Auto Feeder
    handler->registerEventHandler(new RelayHandlerMaxTimed("Feed", GPIO_NUM_14, 5000, true));

    // LED Lights - NOTE: FastLED is Arduino-specific; ensure FastLedHandler works under IDF
    // handler->registerEventHandler(new FastLedHandler("WS2815 - Full white", CRGB(255, 200, 150)));

    // System Reset
    handler->registerEventHandler(new LambdaHandler("SystemReset", [](uICAL::CalendarEntry_ptr evt) {
        Logger::getInstance().info("Resetting system per calendar event");
        esp_restart();
        }));
}


// ------------------ Helper to add handlers --------------------

static void telnetSetup() {

#if TELNET_PORT != 0
    // TODO: adapt TelnetServer and PrinterCollection to work with IDF if they still rely on Arduino primitives
    TelnetServer<TELNET_PORT>::getInstance()->begin();
    PrinterCollection* printerCollection = new PrinterCollection();
    // you may need adapters instead of Serial
    printerCollection->addPrinter(nullptr); // placeholder for serial-like logger adapter
    printerCollection->addPrinter(TelnetServer<23>::getInstance());
    Logger::getInstance().setWriteStrategy(printerCollection);

#if TELNET_WAIT_FOR_CLIENT_SECONDS > 0
    // blink some LED or indicate waiting - replace LED_BUILTIN with real gpio if desired
    Logger::getInstance().info("Waiting for telnet client connection (if configured)");
    for (int i = 0; i < TELNET_WAIT_FOR_CLIENT_SECONDS; ++i) {
        // TODO: toggle a status LED via gpio if needed
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (TelnetServer<TELNET_PORT>::getInstance()->getClientCount() > 0) {
            Logger::getInstance().info("Telnet client connected");
            break;
        }
    }
#endif
#endif

}

// ------------------ app_main / bootstrap --------------------

extern "C" void app_main() {
    // brief initial delay to let caps settle (was sleep(2))
    vTaskDelay(pdMS_TO_TICKS(2000));

    // initialize WiFi
    wifiSetup(WIFI_SSID, WIFI_PASSWORD, true);

    // time sync (SNTP)
    timeSetup(TIMEZONE, NTP_SERVER_1, NTP_SERVER_2);

    telnetSetup();


    // create mutex for calendar handler
    calendar_mutex = xSemaphoreCreateMutex();
    if (!calendar_mutex) {
        Logger::getInstance().error("Failed to create calendar mutex");
        // continue anyway, but racey
    }

    // Kick off calendar download task
    xTaskCreatePinnedToCore(calendarTask, "calendarTask", CALENDAR_TASK_STACK, nullptr, CALENDAR_TASK_PRIO, nullptr, tskNO_AFFINITY);

    // Give initial calendar a moment to populate
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Kick off event processing
    xTaskCreatePinnedToCore(eventProcessingTask, "eventTask", EVENT_TASK_STACK, nullptr, EVENT_TASK_PRIO, nullptr, tskNO_AFFINITY);

    // Kick off OTA task
    xTaskCreatePinnedToCore(otaTask, "otaTask", OTA_TASK_STACK, nullptr, OTA_TASK_PRIO, nullptr, tskNO_AFFINITY);

    // Main task can optionally do supervision, heartbeat, or just idle
    while (true) {
        Logger::getInstance().debug("Main supervisor alive");
        vTaskDelay(pdMS_TO_TICKS(5000));
        // TODO: health checks, restart logic, metrics aggregation, etc.
    }
}
