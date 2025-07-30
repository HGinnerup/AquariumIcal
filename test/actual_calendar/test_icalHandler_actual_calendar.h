#include "common.h"

#include <gtest/gtest.h>


#include "logger.h"
#include "timeUtil.h"
#include "icalIterator.h"
#include "icalHandler.h"

#include "mocks/eventTestHandler.h"

namespace test_icalHandler_actual_calendar {
    IcalIterator_ptr makeIterator(time_t startTime) {
        String calStr =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Google Inc//Google Calendar 70.9054//EN\n"
            "VERSION:2.0\n"
            "CALSCALE:GREGORIAN\n"
            "METHOD:PUBLISH\n"
            "X-WR-CALNAME:AquariumAutomation\n"
            "X-WR-TIMEZONE:Europe/Copenhagen\n"
            "BEGIN:VTIMEZONE\n"
            "TZID:Europe/Copenhagen\n"
            "X-LIC-LOCATION:Europe/Copenhagen\n"
            "BEGIN:DAYLIGHT\n"
            "TZOFFSETFROM:+0100\n"
            "TZOFFSETTO:+0200\n"
            "TZNAME:GMT+2\n"
            "DTSTART:19700329T020000\n"
            "RRULE:FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU\n"
            "END:DAYLIGHT\n"
            "BEGIN:STANDARD\n"
            "TZOFFSETFROM:+0200\n"
            "TZOFFSETTO:+0100\n"
            "TZNAME:GMT+1\n"
            "DTSTART:19701025T030000\n"
            "RRULE:FREQ=YEARLY;BYMONTH=10;BYDAY=-1SU\n"
            "END:STANDARD\n"
            "END:VTIMEZONE\n"
            "BEGIN:VEVENT\n"
            "DTSTART;TZID=Europe/Copenhagen:20250622T070000\n"
            "DTEND;TZID=Europe/Copenhagen:20250622T210000\n"
            "RRULE:FREQ=DAILY\n"
            "DTSTAMP:20250729T103835Z\n"
            "CREATED:20250425T185712Z\n"
            "LAST-MODIFIED:20250622T132057Z\n"
            "SEQUENCE:0\n"
            "STATUS:CONFIRMED\n"
            "SUMMARY:CO2\n"
            "TRANSP:OPAQUE\n"
            "END:VEVENT\n"
            "BEGIN:VEVENT\n"
            "DTSTART;TZID=Europe/Copenhagen:20250622T180000\n"
            "DTEND;TZID=Europe/Copenhagen:20250622T230000\n"
            "RRULE:FREQ=DAILY\n"
            "DTSTAMP:20250729T103835Z\n"
            "CREATED:20250504T150142Z\n"
            "LAST-MODIFIED:20250622T132140Z\n"
            "SEQUENCE:16\n"
            "STATUS:CONFIRMED\n"
            "SUMMARY:WS2815 - Full white\n"
            "TRANSP:OPAQUE\n"
            "END:VEVENT\n"
            "BEGIN:VEVENT\n"
            "DTSTART;TZID=Europe/Copenhagen:20250622T190000\n"
            "DTEND;TZID=Europe/Copenhagen:20250622T190000\n"
            "RRULE:FREQ=DAILY;UNTIL=20250701T215959Z;INTERVAL=2\n"
            "DTSTAMP:20250729T103835Z\n"
            "CREATED:20250622T124632Z\n"
            "LAST-MODIFIED:20250703T083624Z\n"
            "SEQUENCE:0\n"
            "STATUS:CONFIRMED\n"
            "SUMMARY:Feed\n"
            "TRANSP:OPAQUE\n"
            "END:VEVENT\n"
            "BEGIN:VEVENT\n"
            "DTSTART;TZID=Europe/Copenhagen:20250702T190000\n"
            "DTEND;TZID=Europe/Copenhagen:20250702T190000\n"
            "RRULE:FREQ=DAILY;INTERVAL=2\n"
            "DTSTAMP:20250729T103835Z\n"
            "CREATED:20250622T124632Z\n"
            "LAST-MODIFIED:20250703T083658Z\n"
            "SEQUENCE:1\n"
            "STATUS:CONFIRMED\n"
            "SUMMARY:Feed\n"
            "TRANSP:OPAQUE\n"
            "END:VEVENT\n"
            "BEGIN:VEVENT\n"
            "DTSTART;TZID=Europe/Copenhagen:20250714T000000\n"
            "DTEND;TZID=Europe/Copenhagen:20250714T000000\n"
            "RRULE:FREQ=DAILY\n"
            "DTSTAMP:20250729T103835Z\n"
            "CREATED:20250715T204801Z\n"
            "LAST-MODIFIED:20250715T204820Z\n"
            "SEQUENCE:1\n"
            "STATUS:CONFIRMED\n"
            "SUMMARY:SystemReset\n"
            "TRANSP:OPAQUE\n"
            "END:VEVENT\n"
            "BEGIN:VEVENT\n"
            "DTSTART;TZID=Europe/Copenhagen:20250622T070000\n"
            "DTEND;TZID=Europe/Copenhagen:20250622T080000\n"
            "RRULE:FREQ=DAILY\n"
            "DTSTAMP:20250729T103835Z\n"
            "CREATED:20250622T132233Z\n"
            "LAST-MODIFIED:20250716T060131Z\n"
            "SEQUENCE:1\n"
            "STATUS:CONFIRMED\n"
            "SUMMARY:WS2815 - Full white\n"
            "TRANSP:OPAQUE\n"
            "END:VEVENT\n"
            "BEGIN:VEVENT\n"
            "DTSTART;TZID=Europe/Copenhagen:20250622T080000\n"
            "DTEND;TZID=Europe/Copenhagen:20250622T220000\n"
            "RRULE:FREQ=DAILY;UNTIL=20250723T215959Z\n"
            "DTSTAMP:20250729T103835Z\n"
            "CREATED:20250425T185532Z\n"
            "LAST-MODIFIED:20250724T201057Z\n"
            "SEQUENCE:6\n"
            "STATUS:CONFIRMED\n"
            "SUMMARY:Light - Bright\n"
            "TRANSP:OPAQUE\n"
            "END:VEVENT\n"
            "BEGIN:VEVENT\n"
            "DTSTART;TZID=Europe/Copenhagen:20250724T080000\n"
            "DTEND;TZID=Europe/Copenhagen:20250724T220000\n"
            "RRULE:FREQ=DAILY\n"
            "DTSTAMP:20250729T103835Z\n"
            "CREATED:20250425T185532Z\n"
            "LAST-MODIFIED:20250724T201057Z\n"
            "SEQUENCE:6\n"
            "STATUS:CONFIRMED\n"
            "SUMMARY:Light - Bright\n"
            "TRANSP:OPAQUE\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";

        return new_shared_ptr<IcalIterator>(calStr, startTime);
    }


    TEST(ICalHandler, ActualCalendarToday) {
        Logger::getInstance().setLogLevel(LogLevel::WARN);

        struct tm time = timeStruct(2025, 07, 19, 0, 0, 0, true);

        IcalIterator_ptr calIterator = makeIterator(getUnixTimeFromLocal(time));

        IcalHandler* icalHandler = new IcalHandler(calIterator);
        EventTestHandler* ws2815Handler = new EventTestHandler("WS2815 - Full white");
        EventTestHandler* Co2Handler = new EventTestHandler("CO2");
        EventTestHandler* LightHandler = new EventTestHandler("Light - Bright");
        EventTestHandler* FeedHandler = new EventTestHandler("Feed");
        icalHandler->registerEventHandler(ws2815Handler);
        icalHandler->registerEventHandler(Co2Handler);
        icalHandler->registerEventHandler(LightHandler);
        icalHandler->registerEventHandler(FeedHandler);


        for (int days = 0; days < 5; days++, time.tm_mday++) {
            // Nothing yet
            time.tm_hour = 6;
            icalHandler->processEventsUntil(getUnixTimeFromLocal(time));
            EXPECT_EQ(days * 2, ws2815Handler->timesStartCalled);
            EXPECT_EQ(days * 2, ws2815Handler->timesEndCalled);
            EXPECT_EQ(days, Co2Handler->timesEndCalled);
            EXPECT_EQ(days, Co2Handler->timesStartCalled);
            EXPECT_EQ(days, LightHandler->timesStartCalled);
            EXPECT_EQ(days, LightHandler->timesEndCalled);
            EXPECT_EQ(days / 2, FeedHandler->timesStartCalled);
            EXPECT_EQ(days / 2, FeedHandler->timesEndCalled);

            // CO2 and WS2815 start
            time.tm_hour = 7;
            icalHandler->processEventsUntil(getUnixTimeFromLocal(time));
            EXPECT_EQ(days * 2 + 1, ws2815Handler->timesStartCalled);
            EXPECT_EQ(days * 2, ws2815Handler->timesEndCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesStartCalled);
            EXPECT_EQ(days, Co2Handler->timesEndCalled);
            EXPECT_EQ(days, LightHandler->timesStartCalled);
            EXPECT_EQ(days, LightHandler->timesEndCalled);
            EXPECT_EQ(days / 2, FeedHandler->timesStartCalled);
            EXPECT_EQ(days / 2, FeedHandler->timesEndCalled);

            // WS2815 end
            time.tm_hour = 8;
            icalHandler->processEventsUntil(getUnixTimeFromLocal(time));
            EXPECT_EQ(days * 2 + 1, ws2815Handler->timesStartCalled);
            EXPECT_EQ(days * 2 + 1, ws2815Handler->timesEndCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesStartCalled);
            EXPECT_EQ(days, Co2Handler->timesEndCalled);
            EXPECT_EQ(days + 1, LightHandler->timesStartCalled);
            EXPECT_EQ(days, LightHandler->timesEndCalled);
            EXPECT_EQ(days / 2, FeedHandler->timesStartCalled);
            EXPECT_EQ(days / 2, FeedHandler->timesEndCalled);

            // WS2815 start
            time.tm_hour = 18;
            icalHandler->processEventsUntil(getUnixTimeFromLocal(time));
            EXPECT_EQ(days * 2 + 2, ws2815Handler->timesStartCalled);
            EXPECT_EQ(days * 2 + 1, ws2815Handler->timesEndCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesStartCalled);
            EXPECT_EQ(days, Co2Handler->timesEndCalled);
            EXPECT_EQ(days + 1, LightHandler->timesStartCalled);
            EXPECT_EQ(days, LightHandler->timesEndCalled);
            EXPECT_EQ(days / 2, FeedHandler->timesStartCalled);
            EXPECT_EQ(days / 2, FeedHandler->timesEndCalled);

            // Feed every other day
            time.tm_hour = 19;
            icalHandler->processEventsUntil(getUnixTimeFromLocal(time));
            EXPECT_EQ(days * 2 + 2, ws2815Handler->timesStartCalled);
            EXPECT_EQ(days * 2 + 1, ws2815Handler->timesEndCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesStartCalled);
            EXPECT_EQ(days, Co2Handler->timesEndCalled);
            EXPECT_EQ(days + 1, LightHandler->timesStartCalled);
            EXPECT_EQ(days, LightHandler->timesEndCalled);
            EXPECT_EQ(days / 2 + days % 2, FeedHandler->timesStartCalled);
            EXPECT_EQ(days / 2 + days % 2, FeedHandler->timesEndCalled);

            // CO2 end
            time.tm_hour = 21;
            icalHandler->processEventsUntil(getUnixTimeFromLocal(time));
            EXPECT_EQ(days * 2 + 2, ws2815Handler->timesStartCalled);
            EXPECT_EQ(days * 2 + 1, ws2815Handler->timesEndCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesStartCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesEndCalled);
            EXPECT_EQ(days + 1, LightHandler->timesStartCalled);
            EXPECT_EQ(days, LightHandler->timesEndCalled);
            EXPECT_EQ(days / 2 + days % 2, FeedHandler->timesStartCalled);
            EXPECT_EQ(days / 2 + days % 2, FeedHandler->timesEndCalled);

            // Light end
            time.tm_hour = 22;
            icalHandler->processEventsUntil(getUnixTimeFromLocal(time));
            EXPECT_EQ(days * 2 + 2, ws2815Handler->timesStartCalled);
            EXPECT_EQ(days * 2 + 1, ws2815Handler->timesEndCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesStartCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesEndCalled);
            EXPECT_EQ(days + 1, LightHandler->timesStartCalled);
            EXPECT_EQ(days + 1, LightHandler->timesEndCalled);
            EXPECT_EQ(days / 2 + days % 2, FeedHandler->timesStartCalled);
            EXPECT_EQ(days / 2 + days % 2, FeedHandler->timesEndCalled);

            // WS2815 end
            time.tm_hour = 23;
            icalHandler->processEventsUntil(getUnixTimeFromLocal(time));
            EXPECT_EQ(days * 2 + 2, ws2815Handler->timesStartCalled);
            EXPECT_EQ(days * 2 + 2, ws2815Handler->timesEndCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesStartCalled);
            EXPECT_EQ(days + 1, Co2Handler->timesEndCalled);
            EXPECT_EQ(days + 1, LightHandler->timesStartCalled);
            EXPECT_EQ(days + 1, LightHandler->timesEndCalled);
            EXPECT_EQ(days / 2 + days % 2, FeedHandler->timesStartCalled);
            EXPECT_EQ(days / 2 + days % 2, FeedHandler->timesEndCalled);
        }
    };
}
