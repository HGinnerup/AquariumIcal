#pragma once

#include <uICAL.h>
#include <uICAL/stream.h>

#ifndef ARDUINO 
    #include <limits>
    #include <sstream>
#endif

#include "common.h"
#include "logger.h"

class IcalIterator {
protected:
    uICAL::Calendar_ptr calendar;
    uICAL::CalendarIter_ptr uicalIterator;

public:
    IcalIterator(String ical_str, time_t startTime, time_t endTime = std::numeric_limits<time_t>::max()) {
        try {
            #ifdef ARDUINO
                uICAL::istream_String istm(ical_str);
            #else
                std::istringstream ical_str_stream(ical_str);
                uICAL::istream_stl istm(ical_str_stream);
            #endif
            this->calendar = uICAL::Calendar::load(istm);
        }
        catch (uICAL::Error ex) {
            Logger::getInstance().error(ex.message + ": ! Failed loading calendar");
        }

        uICAL::DateTime calBegin(startTime);
        uICAL::DateTime calEnd(endTime);

        this->uicalIterator = uICAL::new_ptr<uICAL::CalendarIter>(this->calendar, calBegin, calEnd);
    }

    uICAL::CalendarEntry_ptr consume() {
        this->uicalIterator->next();
        return this->uicalIterator->current();
    }
};
DEF_PTR_TYPE(IcalIterator)
