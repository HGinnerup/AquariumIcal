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
void timeavailable(struct timeval* t) {
    Logger::getInstance().info("Got time adjustment from NTP!", localTimeString());
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
