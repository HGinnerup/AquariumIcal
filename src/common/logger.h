
#pragma once

#ifndef ARDUINO
#include <iostream>
#endif

#include "common.h"

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void setLogLevel(LogLevel level) {
        logLevel_ = level;
    }

protected:
    void write(const std::string& message) {
        write(message.c_str());
    }

    template<typename T>
    void write(T message) {
        #ifdef ARDUINO
            Serial.print(message); // works for int, float, const char*, etc.
        #else
            std::cout << message;
        #endif
    }
    
    void log_helper(LogLevel level) {
        write("\n");
    }

    template<typename T, typename... Args>
    void log_helper(LogLevel level, const T& first, const Args&... rest) {
        write(first);
        log_helper(level, rest...);
    }
public:
    template<typename... Args>
    void log(LogLevel level, const Args&... messageItems) {
        log_helper(level, "[", logLevelToString(level), "] ", messageItems...);
    }

    template<typename... Args>
    void debug(const Args&... messageItems)  { log(LogLevel::DEBUG, messageItems...); }

    template<typename... Args>
    void info(const Args&... messageItems)  { log(LogLevel::INFO, messageItems...); }

    template<typename... Args>
    void warn(const Args&... messageItems)  { log(LogLevel::WARN, messageItems...); }

    template<typename... Args>
    void error(const Args&... messageItems)  { log(LogLevel::ERROR, messageItems...); }

private:
    Logger() : logLevel_(LogLevel::INFO) {}

    std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERROR: return "ERROR";
            default:              return "UNKNOWN";
        }
    }

    LogLevel logLevel_;
};
