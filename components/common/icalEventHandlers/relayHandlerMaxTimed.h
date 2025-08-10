#pragma once

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include <set>
#include <uICAL.h>

#include "icalEventHandlers/RelayHandler.h"

class RelayHandlerMaxTimed : public RelayHandler {
protected:
    int millisecondsOn;

public:
    RelayHandlerMaxTimed(const std::string& eventName, gpio_num_t pinId, int millisecondsOn, bool invertedState = false) : RelayHandler(eventName, pinId, invertedState), millisecondsOn(millisecondsOn) {

    }

    void onEventStart(uICAL::CalendarEntry_ptr evt) override {
        gpio_set_level(this->pin, !inverted);
        vTaskDelay(pdMS_TO_TICKS(this->millisecondsOn));
        gpio_set_level(this->pin, inverted);
    }

};
