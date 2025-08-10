#pragma once

#include "common.h"
#include <uICAL.h>
#include "driver/gpio.h"

#include "icalEventHandlers/icalEventHandler.h"

class RelayHandler : public IcalEventHandler {
protected:
    gpio_num_t pin;
    bool inverted;

public:
    RelayHandler(const String& eventName, gpio_num_t pinId, bool invertedState = false) : IcalEventHandler(eventName), pin(pinId), inverted(invertedState) {
        gpio_reset_pin(pinId);
        gpio_set_direction(pinId, GPIO_MODE_OUTPUT);
        gpio_set_level(pinId, inverted); // Default state to off
    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        gpio_set_level(this->pin, !inverted);
    }

    void onEventEnd(uICAL::CalendarEntry_ptr evt) override {
        gpio_set_level(this->pin, inverted);
    }
};
