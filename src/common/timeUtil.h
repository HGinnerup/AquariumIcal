#pragma once

#include "time.h"
#include "logger.h"


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

void timeSetup(const char* timezone, const char* ntpServer1, const char* ntpServer2) {
    sntp_set_time_sync_notification_cb(timeavailable);
    configTzTime(timezone, ntpServer1, ntpServer2);
}

time_t getUnixTime() {
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return(0);
    }
    time(&now);

    return now;
}
