#pragma once

#include "common.h"
#include <uICAL.h>


class IcalEventHandler {
public:
    virtual String getEventName() = 0;
    virtual void onEventStart(uICAL::CalendarEntry_ptr evt) = 0;
    virtual void onEventEnd(uICAL::CalendarEntry_ptr evt) = 0;
};
