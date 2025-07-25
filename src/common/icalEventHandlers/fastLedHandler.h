#pragma once

#include <FastLED.h>
#include "common.h"

#include "icalHandler.h"


class FastLedHandler : public IcalEventHandler {
private:
    CRGB color;

public:
    FastLedHandler(const String& eventName, CRGB color) : IcalEventHandler(eventName), color(color) {
        //FastLED.showColor(CRGB(255, 200, 150));
    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        FastLED.showColor(this->color);
    }

    void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
        FastLED.showColor(CRGB(0, 0, 0));
    }
};

