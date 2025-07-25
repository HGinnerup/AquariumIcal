#include "common.h"

#include <gtest/gtest.h>


#include "fakes/calendar/calendar.h"
#include "fakes/calendar/timezone.h"
#include "fakes/calendar/timezone_utc.h"
#include "icalIterator.h"
#include "icalHandler.h"
// #include "icalEventHandlers/lambdaHandler.h"

// #include "mocks/uical/CalendarIter.h"
#include "mocks/eventTestHandler.h"


class CalendarTester {
protected:
    CalendarFakes::Calendar calendarGenerator;

public:
    CalendarTester(String calendarName = "TestCal", CalendarFakes::Timezone_ptr timezone = CalendarFakes::Timezone_utc::getInstance()) : calendarGenerator(CalendarFakes::Calendar(calendarName, timezone)) { }

    void addEvent(String name, CalendarFakes::CalendarDatetime startTime, CalendarFakes::CalendarDatetime endTime) {
        this->calendarGenerator.addEvent(name, startTime, endTime);
    }

    void addEvent(String name, time_t startUTC, time_t endUTC) {

        // This implementation is extremely lazy, and does not support unix-timestamps beyond 60 seconds
        // Feel free to fix
        assert(startUTC < 60); 
        assert(endUTC < 60);

        CalendarFakes::Timezone_ptr tz = this->calendarGenerator.getTimezone();

        this->addEvent(
            name,
            CalendarFakes::CalendarDatetime(1970, 1, 1, 0, 0, startUTC, tz),
            CalendarFakes::CalendarDatetime(1970, 1, 1, 0, 0, endUTC, tz)
        );
    }


    IcalIterator_ptr makeIterator(time_t startTime) {
        return new_shared_ptr<IcalIterator>(this->calendarGenerator.getIcalString(), startTime);
    }
};


TEST(ICalHandler, Basic_Functionality) {
    CalendarTester cal("test");
    cal.addEvent("Ev1", 4, 10);
    IcalIterator_ptr calIterator = cal.makeIterator(0);

    IcalHandler* icalHandler = new IcalHandler(calIterator);
    EventTestHandler* ev1Handler = new EventTestHandler("Ev1");
    icalHandler->registerEventHandler(ev1Handler);

    
    // No early calls
    icalHandler->processEventsUntil(2);
    EXPECT_EQ(0, ev1Handler->timesStartCalled);
    EXPECT_EQ(0, ev1Handler->timesEndCalled);
    
    // Start on time
    icalHandler->processEventsUntil(4);
    EXPECT_EQ(1, ev1Handler->timesStartCalled);
    EXPECT_EQ(0, ev1Handler->timesEndCalled);
    
    // Still going
    icalHandler->processEventsUntil(8);
    EXPECT_EQ(1, ev1Handler->timesStartCalled);
    EXPECT_EQ(0, ev1Handler->timesEndCalled);
    
    // Finish on time
    icalHandler->processEventsUntil(10);
    EXPECT_EQ(1, ev1Handler->timesStartCalled);
    EXPECT_EQ(1, ev1Handler->timesEndCalled);       

    // Nothing wierd later
    icalHandler->processEventsUntil(30);
    EXPECT_EQ(1, ev1Handler->timesStartCalled);
    EXPECT_EQ(1, ev1Handler->timesEndCalled);
}


TEST(ICalHandler, Intersecting_end_and_start) {
    CalendarTester cal("test");
    cal.addEvent("Ev1",  5, 10);
    cal.addEvent("Ev2", 10, 15);
    IcalIterator_ptr calIterator = cal.makeIterator(0);

    IcalHandler* icalHandler = new IcalHandler(calIterator);
    EventTestHandler* ev1Handler = new EventTestHandler("Ev1");
    EventTestHandler* ev2Handler = new EventTestHandler("Ev2");
    icalHandler->registerEventHandler(ev1Handler);
    icalHandler->registerEventHandler(ev2Handler);

    // Only first
    icalHandler->processEventsUntil(9);
    EXPECT_EQ(1, ev1Handler->timesStartCalled);
    EXPECT_EQ(0, ev1Handler->timesEndCalled);
    EXPECT_EQ(0, ev2Handler->timesStartCalled);
    EXPECT_EQ(0, ev2Handler->timesEndCalled);
    
    // Switch
    icalHandler->processEventsUntil(10);
    EXPECT_EQ(1, ev1Handler->timesStartCalled);
    EXPECT_EQ(1, ev1Handler->timesEndCalled);
    EXPECT_EQ(1, ev2Handler->timesStartCalled);
    EXPECT_EQ(0, ev2Handler->timesEndCalled);
    
    // Finish
    icalHandler->processEventsUntil(15);
    EXPECT_EQ(1, ev1Handler->timesStartCalled);
    EXPECT_EQ(1, ev1Handler->timesEndCalled);
    EXPECT_EQ(1, ev2Handler->timesStartCalled);
    EXPECT_EQ(1, ev2Handler->timesEndCalled);
}


TEST(ICalHandler, Intersecting_end_and_start_recurring) {
    CalendarTester cal("test");
    cal.addEvent("Ev1",  5, 10);
    cal.addEvent("Ev2", 10, 15);
    IcalIterator_ptr calIterator = cal.makeIterator(0);

    IcalHandler* icalHandler = new IcalHandler(calIterator);
    EventTestHandler* ev1Handler = new EventTestHandler("Ev1", 500);
    EventTestHandler* ev2Handler = new EventTestHandler("Ev2");
    icalHandler->registerEventHandler(ev1Handler);
    icalHandler->registerEventHandler(ev2Handler);

    const time_t day_seconds = 60*60*24;

    for(int days=0; days<10; days++) {
        icalHandler->processEventsUntil(days * day_seconds);
        EXPECT_EQ(days, ev1Handler->timesStartCalled);
        EXPECT_EQ(days, ev1Handler->timesEndCalled);
        EXPECT_EQ(days, ev2Handler->timesStartCalled);
        EXPECT_EQ(days, ev2Handler->timesEndCalled);

        icalHandler->processEventsUntil(days * day_seconds + 5);
        EXPECT_EQ(days + 1, ev1Handler->timesStartCalled);
        EXPECT_EQ(days, ev1Handler->timesEndCalled);
        EXPECT_EQ(days, ev2Handler->timesStartCalled);
        EXPECT_EQ(days, ev2Handler->timesEndCalled);

        icalHandler->processEventsUntil(days * day_seconds + 10);
        EXPECT_EQ(days + 1, ev1Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev1Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesStartCalled);
        EXPECT_EQ(days, ev2Handler->timesEndCalled);

        icalHandler->processEventsUntil(days * day_seconds + 15);
        EXPECT_EQ(days + 1, ev1Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev1Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesEndCalled);
    }
}


TEST(ICalHandler, Intersecting_end_and_start_multiple_recurring) {
    CalendarTester cal("test");
    cal.addEvent("Ev1",  7,  8); // WS2815
    cal.addEvent("Ev2",  7, 21); // CO2
    cal.addEvent("Ev3",  8, 22); // Bright light
    IcalIterator_ptr calIterator = cal.makeIterator(0);

    IcalHandler* icalHandler = new IcalHandler(calIterator);
    EventTestHandler* ev1Handler = new EventTestHandler("Ev1");
    EventTestHandler* ev2Handler = new EventTestHandler("Ev2");
    EventTestHandler* ev3Handler = new EventTestHandler("Ev3");
    icalHandler->registerEventHandler(ev1Handler);
    icalHandler->registerEventHandler(ev2Handler);
    icalHandler->registerEventHandler(ev3Handler);

    const time_t day_seconds = 60*60*24;

    
    for(int days=0; days<5; days++) {
        icalHandler->processEventsUntil(days * day_seconds + 6);
        EXPECT_EQ(days    , ev1Handler->timesStartCalled);
        EXPECT_EQ(days    , ev1Handler->timesEndCalled);
        EXPECT_EQ(days    , ev2Handler->timesStartCalled);
        EXPECT_EQ(days    , ev2Handler->timesEndCalled);
        EXPECT_EQ(days    , ev3Handler->timesStartCalled);
        EXPECT_EQ(days    , ev3Handler->timesEndCalled);

        icalHandler->processEventsUntil(days * day_seconds + 7);
        EXPECT_EQ(days + 1, ev1Handler->timesStartCalled);
        EXPECT_EQ(days    , ev1Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesStartCalled);
        EXPECT_EQ(days    , ev2Handler->timesEndCalled);
        EXPECT_EQ(days    , ev3Handler->timesStartCalled);
        EXPECT_EQ(days    , ev3Handler->timesEndCalled);

        icalHandler->processEventsUntil(days * day_seconds + 8);
        EXPECT_EQ(days + 1, ev1Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev1Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesStartCalled);
        EXPECT_EQ(days    , ev2Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev3Handler->timesStartCalled);
        EXPECT_EQ(days    , ev3Handler->timesEndCalled);

        icalHandler->processEventsUntil(days * day_seconds + 21);
        EXPECT_EQ(days + 1, ev1Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev1Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev3Handler->timesStartCalled);
        EXPECT_EQ(days    , ev3Handler->timesEndCalled);

        icalHandler->processEventsUntil(days * day_seconds + 22);
        EXPECT_EQ(days + 1, ev1Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev1Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev2Handler->timesEndCalled);
        EXPECT_EQ(days + 1, ev3Handler->timesStartCalled);
        EXPECT_EQ(days + 1, ev3Handler->timesEndCalled);
    }
}
