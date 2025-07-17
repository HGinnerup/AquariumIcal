#pragma once

#include <FastLED.h>

#include "icalHandler.h"


class FastLedHandler : public IcalEventHandler {
private:
    String name;
    CRGB color;

public:
    FastLedHandler(String eventName, CRGB color) : name(eventName), color(color) {
        //FastLED.showColor(CRGB(255, 200, 150));
    }

    String getEventName() override {
        return name;
    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        FastLED.showColor(this->color);
    }

    void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
        FastLED.showColor(CRGB(0, 0, 0));
    }
};

