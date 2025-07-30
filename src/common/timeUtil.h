#pragma once

#include "time.h"
#include "logger.h"

#ifndef ARDUIONO
#include <chrono>
#endif

#ifdef ARDUINO
String localTimeString() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "No time available (yet)";
    }
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer);
}


// Callback function (gets called when time adjusts via NTP)
bool TIME_HAS_BEEN_SET = false;
void timeavailable(struct timeval* t) {
    Logger::getInstance().info("Got time adjustment from NTP! ", localTimeString());
    TIME_HAS_BEEN_SET = true;
}
#endif

void timeSetup(const char* timezone, const char* ntpServer1, const char* ntpServer2) {
#ifdef ARDUINO
    sntp_set_time_sync_notification_cb(timeavailable);
    configTzTime(timezone, ntpServer1, ntpServer2);
#endif
}

time_t getUnixTime() {
#ifdef ARDUINO

    if (!TIME_HAS_BEEN_SET)
        return 0;

    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return(0);
    }
    time(&now);

    return now;
#else
    const auto clock = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(clock.time_since_epoch()).count();
#endif
}


struct tm timeStruct(int year, int month, int day, int hour, int minute, int second, bool isSummerTime) {
    struct tm timeInfo = { 0 };
    timeInfo.tm_year = year - 1900; // tm_year is years since 1900
    timeInfo.tm_mon = month - 1;    // tm_mon is 0-based
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = hour;
    timeInfo.tm_min = minute;
    timeInfo.tm_sec = second;
    timeInfo.tm_isdst = isSummerTime;

    return timeInfo;
}

struct tm* timeStructLocal(time_t unixTime) {
    return localtime(&unixTime);
}


time_t getUnixTimeFromLocal(struct tm localTimeStruct) {
    return mktime(&localTimeStruct);
}

String toIsoTimestamp(struct tm* timeInfo) {
    char timeStr[sizeof "2025-07-30T15:44:50Z"];
    strftime(timeStr, sizeof timeStr, "%FT%TZ", timeInfo);

    return String(timeStr);
}

String toIsoTimestampLocal(time_t unixTime) {
    struct tm* timeInfo = timeStructLocal(unixTime);
    return toIsoTimestamp(timeInfo);
}

