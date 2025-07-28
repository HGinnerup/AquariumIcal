#pragma once

#include <FastLED.h>
#include "common.h"

#include "icalHandler.h"


class FastLedHandler : public IcalEventHandler {
protected:
    CRGB onColor;
    CRGB offColor;

    void write(CRGB color, int howManyTimesToCall=5, int delayMs=50) {
        for(int i=0; i<howManyTimesToCall; i++) {
            FastLED.showColor(color);

            if(i < howManyTimesToCall - 1) { // Don't delay after the last attempt
                delay(delayMs);
            }
        }
    }

        
public:
    FastLedHandler(const String& eventName, CRGB onColor, CRGB offColor = CRGB(0, 0, 0)) : IcalEventHandler(eventName), onColor(onColor), offColor(offColor) {
        this->write(offColor, 1); // Turn off in case they were already lit
    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        this->write(this->onColor);
    }

    void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
        this->write(this->offColor);
    }
};

