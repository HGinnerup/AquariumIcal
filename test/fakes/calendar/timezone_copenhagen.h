#pragma once

#include "common.h"
#include "timezone.h"

namespace CalendarFakes {

    class Timezone_copenhagen : public Timezone {
    public:

        Timezone_copenhagen() : Timezone("Europe/Copenhagen") { }

        static Timezone_copenhagen& getInstance() {
            static Timezone_copenhagen instance;
            return instance;
        }

        String getIcalString() {
            return String(
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
            );
        }
    };
}
