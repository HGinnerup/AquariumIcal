#include "common.h"

#include <unity.h>

#include "icalEventHandlers/lambdaHandler.h"

IcalEventHandler* eventHandler;

int timesStartCalled = 0;
int timesEndCalled = 0;

void setUp(void) {
    // set stuff up here
    eventHandler = new LambdaHandler("Test Event",
        [](uICAL::CalendarEntry_ptr evt) {
            timesStartCalled++;
        },
        [](uICAL::CalendarEntry_ptr evt) {
            timesEndCalled++;
        });
}

void tearDown(void) {
    // clean stuff up here
}

void test_eventBegin(void) {
    int prevTimesStartCalled = timesStartCalled;
    int prevTimesEndCalled = timesEndCalled;

    eventHandler->onEventStart(nullptr);

    TEST_ASSERT_EQUAL(prevTimesStartCalled + 1, timesStartCalled);
    TEST_ASSERT_EQUAL(prevTimesEndCalled, timesEndCalled);
}

void test_eventEnd(void) {
    int prevTimesStartCalled = timesStartCalled;
    int prevTimesEndCalled = timesEndCalled;

    eventHandler->onEventEnd(nullptr);

    TEST_ASSERT_EQUAL(prevTimesStartCalled, timesStartCalled);
    TEST_ASSERT_EQUAL(prevTimesEndCalled + 1, timesEndCalled);
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_eventBegin);
    RUN_TEST(test_eventEnd);
    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    process();
}

void loop() {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}

#else

int main(int argc, char** argv) {
    process();
    return 0;
}

#endif