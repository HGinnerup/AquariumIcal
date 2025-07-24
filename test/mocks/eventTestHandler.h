#pragma once

#include <thread>
#include <chrono>
#include "icalEventHandlers/icalEventHandler.h"

class EventTestHandler : public IcalEventHandler {

public:
    int timesStartCalled;
    int timesEndCalled;
    int executionTimeMs;

    EventTestHandler(String eventName, int executionTimeMs = 0) : IcalEventHandler(eventName), timesStartCalled(0), timesEndCalled(0), executionTimeMs(executionTimeMs) { }
    
    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        this->timesStartCalled++;
        std::this_thread::sleep_for(std::chrono::milliseconds(this->executionTimeMs));
    }

    void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
        this->timesEndCalled++;
    }
    
};
