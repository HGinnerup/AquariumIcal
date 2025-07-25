#pragma once

#include "common.h"
#include <set>
#include <uICAL.h>

#include "icalEventHandlers/icalEventHandler.h"

class RelayHandler : public IcalEventHandler {
protected:
    int pin;
    bool inverted;

public:
    RelayHandler(const String& eventName, int pinId, bool invertedState = false) : IcalEventHandler(eventName), pin(pinId), inverted(invertedState) {
        pinMode(pinId, OUTPUT);
        digitalWrite(this->pin, inverted); // Default state to off
    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        digitalWrite(this->pin, !inverted);
    }

    void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
        digitalWrite(this->pin, inverted);
    }
};
