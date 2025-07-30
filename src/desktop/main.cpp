// #include <Arduino.h>


#include <memory>
#include <uICAL.h>
// #include <FastLED.h>

#include "common.h"
#include "logger.h"
#include "webConnectivity.h"
#include "timeUtil.h"
#include "icalHandler.h"

//#include "icalEventHandlers/relayHandler.h"
//#include "icalEventHandlers/relayHandlerMaxTimed.h"
//#include "icalEventHandlers/fastLedHandler.h"
#include "icalEventHandlers/lambdaHandler.h"

// Copy config_example.h to config.h and edit with your credentials
#include "config.h"

Logger logger = Logger::getInstance();
void ical_setup();
void setup() {
    //sleep(2); // Allow capacitors to charge before turning on antennas
    //Serial.begin(115200);

    wifiSetup(WIFI_SSID, WIFI_PASSWORD);

    timeSetup(TIMEZONE, NTP_SERVER_1, NTP_SERVER_2);

    ical_setup();
}


IcalIterator_ptr ical_download_calendar() {
    String ical_str = httpsGet(ICAL_URL, ROOT_CA_CERTIFICATE);

    Logger::getInstance().info("Ical String: ", ical_str);

    time_t now = getUnixTime();
    IcalIterator_ptr icalIterator = new_shared_ptr<IcalIterator>(ical_str, now);

    return icalIterator;
}


IcalIterator_ptr icalIterator;
IcalHandler* icalHandler;
void ical_setup() {
    icalIterator = ical_download_calendar();
    icalHandler = new IcalHandler(icalIterator);

    // Top light
    icalHandler->registerEventHandler(new LambdaHandler("Light - Bright", [](uICAL::CalendarEntry_ptr evt) {}));

    // CO2
    icalHandler->registerEventHandler(new LambdaHandler("CO2", [](uICAL::CalendarEntry_ptr evt) {}));

    // Auto Feeder
    icalHandler->registerEventHandler(new LambdaHandler("Feed", [](uICAL::CalendarEntry_ptr evt) {}));

    // LED Lights
    icalHandler->registerEventHandler(new LambdaHandler("WS2815 - Full white", [](uICAL::CalendarEntry_ptr evt) {}));

    // System Reset
    icalHandler->registerEventHandler(new LambdaHandler("SystemReset", [](uICAL::CalendarEntry_ptr evt) {
        Logger::getInstance().info("Resetting");
#ifdef ARDUINO
        ESP.restart();
#endif
        }));

}


void loop() {
    time_t current_time = getUnixTime();

    icalHandler->processEventsUntil(current_time);

    time_t next_event_time = icalHandler->getTimeOfNextEvent();
    time_t wait_time = next_event_time - current_time;

    Logger::getInstance().info("Current time: ", toIsoTimestampLocal(current_time));
    Logger::getInstance().info("Time for next event: ", toIsoTimestampLocal(next_event_time));
    Logger::getInstance().info("Waiting for ", wait_time, " seconds");

    delay(wait_time * 1000);
}



int main() {
    setup();
    while (true)
        loop();

    return 0;
}
