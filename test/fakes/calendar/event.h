#pragma once

#include "common.h"
#include "base.h"

#include "calendarDatetime.h"

namespace CalendarFakes {
    class Event : public Base {
    protected:
        String name;
        CalendarDatetime startTime;
        CalendarDatetime endTime;
    public:
        Event(String name, CalendarDatetime startTime, CalendarDatetime endTime) : name(name), startTime(startTime), endTime(endTime) { }

        String getIcalString() {
            return String(
                "BEGIN:VEVENT\n"
                "DTSTART" + this->startTime.getIcalString() + "\n"
                "DTEND" + this->endTime.getIcalString() + "\n"
                "RRULE:FREQ=DAILY\n"
                "DTSTAMP:20250720T121006Z\n"
                "UID:703b6i5vc41goapnt5e1j0q51v@google.com\n"
                "CREATED:20250425T185712Z\n"
                "LAST-MODIFIED:20250622T132057Z\n"
                "SEQUENCE:0\n"
                "STATUS:CONFIRMED\n"
                "SUMMARY:" + name + "\n"
                "TRANSP:OPAQUE\n"
                "END:VEVENT"
            );
        }
    };
}
