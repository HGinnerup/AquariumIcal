#pragma once

#include "common.h"
#include <set>
#include <uICAL.h>

#include "icalEventHandlers/RelayHandler.h"

class RelayHandlerMaxTimed : public RelayHandler {
protected:
    int millisecondsOn;

public:
    RelayHandlerMaxTimed(String eventName, int pinId, int millisecondsOn, bool invertedState = false) : RelayHandler(eventName, pinId, invertedState), millisecondsOn(millisecondsOn) {

    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        digitalWrite(this->pin, !inverted);
        delay(this->millisecondsOn);
        digitalWrite(this->pin, inverted);
    }

};
