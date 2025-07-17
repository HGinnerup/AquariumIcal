#pragma once

#include <string>
#include <set>
#include <uICAL.h>

#include "icalEventHandlers/icalEventHandler.h"

class LambdaHandler : public IcalEventHandler {
protected:
    String name;
    void (*_onEventStart)(uICAL::CalendarEntry_ptr evt);
    void (*_onEventEnd)(uICAL::CalendarEntry_ptr evt);


public:
    LambdaHandler(String eventName, void (*onEventStart)(uICAL::CalendarEntry_ptr evt) = NULL, void (*onEventEnd)(uICAL::CalendarEntry_ptr evt) = NULL) : name(eventName), _onEventStart(onEventStart), _onEventEnd(onEventEnd) {

    }

    String getEventName() override {
        return name;
    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        if (this->_onEventStart != NULL) {
            this->_onEventStart(evt);
        }
    }

    void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
        if (this->_onEventEnd != NULL) {
            this->_onEventEnd(evt);
        }
    }
};
