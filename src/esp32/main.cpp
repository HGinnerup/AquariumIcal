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


Logger logger = Logger::getInstance();
void ical_setup();
void setup() {
    sleep(2); // Allow capacitors to charge before turning on antennas
    Serial.begin(115200);

    wifiSetup(WIFI_SSID, WIFI_PASSWORD);

    timeSetup(TIMEZONE, NTP_SERVER_1, NTP_SERVER_2);

    ical_setup();
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
    FastLED.showColor(CRGB(0, 0, 0)); // Turn off in case they were already lit
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

    delay(wait_time * 1000);
}



