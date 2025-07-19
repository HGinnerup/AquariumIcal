#pragma once

#include "common.h"
#include <uICAL.h>


class IcalEventHandler {
protected:
    String name;
    IcalEventHandler(String eventName) : name(eventName) {}

public:
    virtual String getEventName() {
        return this->name;
    }
    virtual void onEventStart(uICAL::CalendarEntry_ptr evt) = 0;
    virtual void onEventEnd(uICAL::CalendarEntry_ptr evt) = 0;
};
