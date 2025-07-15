
#include "esp_sntp.h"
#include <uICAL.h>
#include <FastLED.h>

#include "webConnectivity.h"
#include "timeUtil.h"
#include "icalHandler.h"
#include "icalFastledEventHandler.h"


const char *ssid = "WIFI NAME";
const char *password = "WIFI PASSWORD";

const char* ical_url = "URL FOR CALENDAR.ICS";


#define TZ_Europe_Copenhagen	"CET-1CEST,M3.5.0,M10.5.0/3"
#define TIMEZONE TZ_Europe_Copenhagen   // Find timezones at https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h


const char *rootCACertificate = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw
MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp
Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA
A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo
27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w
Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw
TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl
qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH
szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8
Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk
MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92
wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p
aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN
VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID
AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E
FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb
C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe
QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy
h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4
7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J
ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef
MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/
Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT
6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ
0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm
2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb
bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c
-----END CERTIFICATE-----
)string_literal"; // Root certificate, valid until 22/06/2036


void ical_setup();
void setup() {
    sleep(2); // Allow capacitors to charge before turning on antennas
    Serial.begin(115200);

    wifiSetup(ssid, password);

    const char *ntpServer1 = "pool.ntp.org";
    const char *ntpServer2 = "time.nist.gov";
    timeSetup(TIMEZONE, ntpServer1, ntpServer2);

    ical_setup();
}


uICAL::CalendarIter_ptr ical_download_calendar() {

    String ical_str = httpsGet(ical_url, rootCACertificate);

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

    icalHandler->registerEventHandler(new RelayHandler("Light - Bright", 32, true));
    icalHandler->registerEventHandler(new RelayHandler("CO2", 33, true));
    icalHandler->registerEventHandler(new RelayHandlerMaxTimed("Feed", 14, 5000, true));

    CRGB leds[144];
    FastLED.addLeds<WS2815, 26, RGB>(leds, 144);
    FastLED.showColor(CRGB(0, 0, 0)); // Turn off in case they were already lit
    icalHandler->registerEventHandler(new FastLedIcalHandler("WS2815 - Full white", CRGB(255, 200, 150)));
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



