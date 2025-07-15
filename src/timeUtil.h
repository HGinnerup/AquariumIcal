#pragma once

#include "time.h"



void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}


// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
    Serial.println("Got time adjustment from NTP!");
    printLocalTime();
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


