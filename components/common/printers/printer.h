#pragma once

#include <cstdint>
#include <cstddef>

class Printer {
public:
    virtual size_t write(uint8_t ch) = 0;
    virtual size_t write(const uint8_t* buffer, const size_t size) = 0;

    virtual void flush() = 0;
};
