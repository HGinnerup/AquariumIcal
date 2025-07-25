#pragma once

#include <time.h>
#include <ctime>
#include <iostream>
// #include <iterator>
#include <locale>

#include <gtest/gtest.h>

#include "common.h"
#include "base.h"
#include "timezone.h"

#include "logger.h"

namespace CalendarFakes {
    class CalendarDatetime : Base {
    public:

        struct tm timeinfo;
        Timezone_ptr timezone;

        String getIcalString() {
            // Example: TZID=Europe/Copenhagen:20250622T210000
            char timeString[17] = ":"; // 17 == size(":yyyymmddThhmmss")
            std::strftime(timeString+1, 16,
                "%Y%m%dT%H%M%S", &(this->timeinfo));

            if(this->timezone != nullptr) {
                return ";TZID=" + this->timezone->getName() + timeString;
            }
            else {
                return timeString;
            }
        }


        CalendarDatetime(struct tm timeinfo, Timezone_ptr timezone = nullptr) : timeinfo(timeinfo), timezone(timezone) { }

        CalendarDatetime(const int year, const int month, const int day, const int hour, const int minute, const int second, Timezone_ptr timezone = nullptr) : timezone(timezone) {
            this->timeinfo = {0};
            this->timeinfo.tm_year = year - 1900; // tm_year is years since 1900
            this->timeinfo.tm_mon = month - 1;    // tm_mon is 0-based
            this->timeinfo.tm_mday = day;
            this->timeinfo.tm_hour = hour;
            this->timeinfo.tm_min = minute;
            this->timeinfo.tm_sec = second;
        }


        time_t getUnixTime() {

            #ifdef _WIN32 
                return _mkgmtime(&(this->timeinfo));
            #elif __linux__ 
                return timegm(&(this->timeinfo));
            #else
                #error UNKNOWN PLATFORM
            #endif


            // return mktime(&(this->timeinfo));
        }
    };

    TEST(CalendarDatetime, getUnixTime) {
        CalendarDatetime time_0(1970, 1, 1, 0, 0, 0);
        EXPECT_EQ(0, time_0.getUnixTime());

        CalendarDatetime time_25(1970, 1, 1, 0, 0, 25);
        EXPECT_EQ(25, time_25.getUnixTime());
    }
}
