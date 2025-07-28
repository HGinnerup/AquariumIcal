#include <Arduino.h>

#include "esp_sntp.h"
#include <memory>
#include <uICAL.h>
#include <FastLED.h>

#include "common.h"
#include "logger.h"
#include "webConnectivity.h"
#include "timeUtil.h"
#include "icalHandler.h"

#include "icalEventHandlers/relayHandler.h"
#include "icalEventHandlers/relayHandlerMaxTimed.h"
#include "icalEventHandlers/fastLedHandler.h"
#include "icalEventHandlers/lambdaHandler.h"

// Copy config_example.h to config.h and edit with your credentials
#include "config.h"

#include "ota.h"
#include "printers/telnetServer.h"


void ical_setup();
void setup() {
    sleep(2); // Allow capacitors to charge before turning on antennas
    Serial.begin(115200);
    Logger::getInstance().setWriteStrategy(&Serial);

    wifiSetup(WIFI_SSID, WIFI_PASSWORD);

#if TELNET_PORT != 0
    TelnetServer<TELNET_PORT>::getInstance()->begin();
    Logger::getInstance().setWriteStrategy(TelnetServer<23>::getInstance());

#if TELNET_WAIT_FOR_CLIENT_SECONDS > 0
    pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < TELNET_WAIT_FOR_CLIENT_SECONDS; i++) {
        digitalWrite(LED_BUILTIN, i % 2);
        delay(1000);
        if (TelnetServer<TELNET_PORT>::getInstance()->getClientCount() > 0) {
            Logger::getInstance().info("Telnet client connected");
            break;
        }
    }
    digitalWrite(LED_BUILTIN, LOW);

#endif
#endif

    timeSetup(TIMEZONE, NTP_SERVER_1, NTP_SERVER_2);

    ical_setup();

    otaSetup(OTA_PORT, ESP_NETWORK_HOSTNAME, OTA_PASSWORD);
}


IcalIterator_ptr ical_download_calendar() {
    String ical_str = httpsGet(ICAL_URL, ROOT_CA_CERTIFICATE);

    time_t now = getUnixTime();
    while(now == 0) {
        Logger::getInstance().info("Waiting for time adjustment");
        delay(1000);
        now = getUnixTime();
    }
    return new_shared_ptr<IcalIterator>(ical_str, now);
}


IcalHandler* icalHandler;
void ical_setup() {
    IcalIterator_ptr icalIterator = ical_download_calendar();
    icalHandler = new IcalHandler(icalIterator);

    // Top light
    icalHandler->registerEventHandler(new RelayHandler("Light - Bright", 32, true));

    // CO2
    icalHandler->registerEventHandler(new RelayHandler("CO2", 33, true));

    // Auto Feeder
    icalHandler->registerEventHandler(new RelayHandlerMaxTimed("Feed", 14, 5000, true));

    // LED Lights
    CRGB leds[144];
    FastLED.addLeds<WS2815, 26, RGB>(leds, 144);
    icalHandler->registerEventHandler(new FastLedHandler("WS2815 - Full white", CRGB(255, 200, 150)));

    // System Reset
    icalHandler->registerEventHandler(new LambdaHandler("SystemReset", [](uICAL::CalendarEntry_ptr evt) {
        Serial.println("Resetting");

        ESP.restart();
        }));

}


void loop() {
    time_t current_time = getUnixTime();

    icalHandler->processEventsUntil(current_time);

    time_t next_event_time = icalHandler->getTimeOfNextEvent();
    time_t wait_time = next_event_time - current_time;

    Logger::getInstance().info("Current time: ", current_time);
    Logger::getInstance().info("Time for next event: ", next_event_time);
    Logger::getInstance().info("Waiting for ", wait_time, " seconds");

    while (getUnixTime() < next_event_time - 2000) {
        otaLoop();
        delay(1000);

        current_time = getUnixTime();
        wait_time = next_event_time - current_time;

        icalHandler->printEventQueue();
        Logger::getInstance().info("\tCurrent time: ", current_time,
            "\r\n\tTime for next event: ", next_event_time,
            "\r\n\tWaiting for ", wait_time, " seconds\r\n");
    }
    delay(next_event_time - getUnixTime());
}



