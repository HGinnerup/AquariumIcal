#pragma once

#include "common.h"

namespace CalendarFakes {
    class Base {
    public:
        virtual String getIcalString() = 0;
    };
}
