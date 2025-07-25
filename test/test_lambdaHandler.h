#include "common.h"

#include <gtest/gtest.h>

#include "icalEventHandlers/lambdaHandler.h"

namespace Test_LambdaHandler {
    int timesStartCalled = 0;
    int timesEndCalled = 0;

    TEST(LambdaHandlerTest, EventBegin) {
        timesStartCalled = 0;
        timesEndCalled = 0;
        IcalEventHandler* eventHandler = new LambdaHandler("Test Event",
            [](uICAL::CalendarEntry_ptr evt) {
                timesStartCalled++;
            },
            [](uICAL::CalendarEntry_ptr evt) {
                timesEndCalled++;
            });

        eventHandler->onEventStart(nullptr);

        EXPECT_EQ(1, timesStartCalled);
        EXPECT_EQ(0, timesEndCalled);
    }

    TEST(LambdaHandlerTest, EventEnd) {
        timesStartCalled = 0;
        timesEndCalled = 0;
        IcalEventHandler* eventHandler = new LambdaHandler("Test Event",
            [](uICAL::CalendarEntry_ptr evt) {
                timesStartCalled++;
            },
            [](uICAL::CalendarEntry_ptr evt) {
                timesEndCalled++;
            });

        eventHandler->onEventEnd(nullptr);

        EXPECT_EQ(0, timesStartCalled);
        EXPECT_EQ(1, timesEndCalled);
    }
}