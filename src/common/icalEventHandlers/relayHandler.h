#pragma once

#include "common.h"
#include <set>
#include <uICAL.h>

#include "icalEventHandlers/icalEventHandler.h"

class RelayHandler : public IcalEventHandler {
protected:
    String name;
    int pin;
    bool inverted;

public:
    RelayHandler(String eventName, int pinId, bool invertedState = false) : name(eventName), pin(pinId), inverted(invertedState) {
        pinMode(pinId, OUTPUT);
        digitalWrite(this->pin, inverted); // Default state to off
    }

    String getEventName() override {
        return name;
    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        digitalWrite(this->pin, !inverted);
    }

    void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
        digitalWrite(this->pin, inverted);
    }
};
