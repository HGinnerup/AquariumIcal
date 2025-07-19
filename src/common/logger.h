
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

    void log(LogLevel level, const String& message) {
        if (level >= logLevel_) {
#if defined(ARDUINO)
            Serial.print("[");
            Serial.print(logLevelToString(level).c_str());
            Serial.print("] ");
            Serial.println(message.c_str());
#else
            std::cout << "[" << logLevelToString(level) << "] " << message << std::endl;
#endif
        }
    }

    void debug(const String& message) { log(LogLevel::DEBUG, message); }
    void info(const String& message)  { log(LogLevel::INFO, message); }
    void warn(const String& message)  { log(LogLevel::WARN, message); }
    void error(const String& message) { log(LogLevel::ERROR, message); }

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
