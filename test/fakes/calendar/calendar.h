#pragma once

// #include <vector.h>

#include "uICAL.h"

#include "common.h"
#include "base.h"
#include "event.h"
#include "timezone.h"


namespace CalendarFakes {
    class Calendar : Base {
    protected:
        String name;
        Timezone_ptr timezone;
        std::vector<Event> events;

        String getEventsString() {
            String result("");
            for (auto event : this->events) {
                result += event.getIcalString() + "\n";
            }
            return result;
        }

    public:
        Calendar(String name, Timezone_ptr timezone) : name(name), timezone(timezone) {}
        Calendar(String name) : name(name) {}

        String getName() {
            return this->name;
        }

        Timezone_ptr getTimezone() {
            return this->timezone;
        }

        String getIcalString() {
            return String(
                "BEGIN:VCALENDAR\n"
                "PRODID:-//Google Inc//Google Calendar 70.9054//EN\n"
                "VERSION:2.0\n"
                "CALSCALE:GREGORIAN\n"
                "METHOD:PUBLISH\n"
                "X-WR-CALNAME:" + this->name + "\n"
                + (this->timezone != nullptr ? this->timezone->getIcalString() + "\n" : "") +
                +this->getEventsString().c_str() +
                "END:VCALENDAR");
        }

        void addEvent(String name, CalendarDatetime startTime, CalendarDatetime endTime) {
            this->events.emplace_back(name, startTime, endTime);
        }
    };
    DEF_PTR_TYPE(Calendar)
}

