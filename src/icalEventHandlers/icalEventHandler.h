#pragma once

#include <uICAL.h>


class IcalEventHandler {
public:
    virtual String getEventName();
    virtual void onEventStart(uICAL::CalendarEntry_ptr evt);
    virtual void onEventEnd(uICAL::CalendarEntry_ptr evt);
};
