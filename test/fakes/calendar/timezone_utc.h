#pragma once

#include "common.h"
#include "timezone.h"

namespace CalendarFakes {

    class Timezone_utc : public Timezone {
    public:

        Timezone_utc() : Timezone("UTC") { }
        
        static std::shared_ptr<Timezone_utc> getInstance() {
            static std::shared_ptr<Timezone_utc> instance(new Timezone_utc);
            return instance;
        }

        String getIcalString() {
            return String(
                "BEGIN:VTIMEZONE\n"
                "TZID:UTC\n"
                "X-LIC-LOCATION:UTC\n"
                "BEGIN:DAYLIGHT\n"
                "TZOFFSETFROM:+0000\n"
                "TZOFFSETTO:+0000\n"
                "TZNAME:GMT+0\n"
                "DTSTART:19700101T000000\n"
                "RRULE:FREQ=YEARLY;BYMONTH=10;BYDAY=-1SU\n"
                "END:DAYLIGHT\n"
                "BEGIN:STANDARD\n"
                "TZOFFSETFROM:+0000\n"
                "TZOFFSETTO:+0000\n"
                "TZNAME:GMT+0\n"
                "DTSTART:19700101T000000\n"
                "RRULE:FREQ=YEARLY;BYMONTH=10;BYDAY=-1SU\n"
                "END:STANDARD\n"
                "END:VTIMEZONE"
            );
        }
    };
    DEF_PTR_TYPE(Timezone_utc)
}
