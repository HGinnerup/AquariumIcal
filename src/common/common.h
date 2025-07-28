#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
using String = std::string;

#include <thread>
#include <chrono>
#endif
#include <memory.h>


#define DEF_PTR_TYPE(T) \
    /* class T; */ \
    using T##_ptr = std::shared_ptr<T>;

template<typename T, typename... Args> \
inline std::unique_ptr<T> new_unique_ptr(Args... args) { return std::unique_ptr<T>(new T(args...)); }

template<typename T, typename... Args> \
inline std::shared_ptr<T> new_shared_ptr(Args... args) { return std::shared_ptr<T>(new T(args...)); }

#ifndef ARDUINO
void delay(time_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
#endif
