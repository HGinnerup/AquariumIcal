#pragma once

#include "common.h"
#include "base.h"

namespace CalendarFakes {

    class Timezone : public Base {
    protected:
        String name;

    public:
        Timezone(String name) : name(name) {}

        String getName() {
            return this->name;
        }

        String getIcalString() {
            return String(
                "X-WR-TIMEZONE:" + this->name + "\n"
                "BEGIN:VTIMEZONE\n"
                "TZID:" + this->name + "\n"
                "X-LIC-LOCATION:" + this->name + "\n"
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
                "END:VTIMEZONE"
            );
        }
    };

    DEF_PTR_TYPE(Timezone)
}
