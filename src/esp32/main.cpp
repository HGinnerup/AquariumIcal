
#include "esp_sntp.h"
#include <uICAL.h>
#include <FastLED.h>

#include "webConnectivity.h"
#include "timeUtil.h"
#include "icalHandler.h"

#include "icalEventHandlers/relayHandler.h"
#include "icalEventHandlers/relayHandlerMaxTimed.h"
#include "icalEventHandlers/fastLedHandler.h"
#include "icalEventHandlers/lambdaHandler.h"

// Copy config_example.h to config.h and edit with your credentials
#include "config.h"


void ical_setup();
void setup() {
    sleep(2); // Allow capacitors to charge before turning on antennas
    Serial.begin(115200);

    wifiSetup(WIFI_SSID, WIFI_PASSWORD);

    timeSetup(TIMEZONE, NTP_SERVER_1, NTP_SERVER_2);

    ical_setup();
}


uICAL::CalendarIter_ptr ical_download_calendar() {

    String ical_str = httpsGet(ICAL_URL, ROOT_CA_CERTIFICATE);

    uICAL::Calendar_ptr cal = nullptr;
    try {
        uICAL::istream_String istm(ical_str);
        cal = uICAL::Calendar::load(istm);
    }
    catch (uICAL::Error ex) {
        Serial.print(ex.message.c_str());
        Serial.println(": ! Failed loading calendar");
    }

    time_t now = getUnixTime();

    uICAL::DateTime calBegin(now);
    //uICAL::DateTime calEnd(now + 86400);
    uICAL::DateTime calEnd(std::numeric_limits<time_t>::max());
    Serial.print("Endtime: ");
    Serial.println(calEnd.as_str().c_str());

    return uICAL::new_ptr<uICAL::CalendarIter>(cal, calBegin, calEnd);
}


uICAL::CalendarIter_ptr icalEventIterator;
IcalHandler* icalHandler;
void ical_setup() {
    icalEventIterator = ical_download_calendar();
    icalHandler = new IcalHandler(icalEventIterator, true);

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

    Serial.print("Current time: ");
    Serial.println(current_time);

    Serial.print("Time for next event: ");
    Serial.println(next_event_time);

    Serial.print("Waiting for ");
    Serial.print(wait_time);
    Serial.println(" seconds");

    delay(wait_time * 1000);
}



