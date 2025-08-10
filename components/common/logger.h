
#pragma once

#ifndef ARDUINO
#include <iostream>
#endif

#ifdef ARDUINO
#include <Print.h>
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

#pragma region Log Level
protected:
    LogLevel logLevel;
    std::string logLevelToString(LogLevel level) {
        switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default:              return "UNKNOWN";
        }
    }
public:
    void setLogLevel(LogLevel level) {
        logLevel = level;
    }
#pragma endregion

#pragma region outputPrinter
#ifdef ARDUINO
protected:
    Print* printer;
public:
    void setWriteStrategy(Print* printer) {
        this->printer = printer;
    }
#endif
protected:
    void write(const char* message) {
#ifdef ARDUINO
        printer->print(message); // works for int, float, const char*, etc.
#else
        std::cout << message;
#endif
    }

    void write(const std::string& message) {
        write(message.c_str());
    }

    template<typename T>
    void write(T message) {
#ifdef ARDUINO
        printer->print(message); // works for int, float, const char*, etc.
#else
        std::cout << message;
#endif
    }

#pragma endregion

#pragma region Logger calls
public:
    template<typename... Args>
    void log(LogLevel level, const Args&... messageItems) {
        if (level < this->logLevel)
            return;

        log_helper(level, "[", logLevelToString(level), "] ", messageItems...);
    }

    template<typename... Args>
    void debug(const Args&... messageItems) { log(LogLevel::DEBUG, messageItems...); }

    template<typename... Args>
    void info(const Args&... messageItems) { log(LogLevel::INFO, messageItems...); }

    template<typename... Args>
    void warn(const Args&... messageItems) { log(LogLevel::WARN, messageItems...); }

    template<typename... Args>
    void error(const Args&... messageItems) { log(LogLevel::ERROR, messageItems...); }

#pragma endregion

#pragma region log_helper
protected:
    void log_helper(LogLevel level) {
#ifdef ARDUINO
        printer->println();
#else
        std::cout << std::endl << std::flush;
#endif

    }

    template<typename T, typename... Args>
    void log_helper(LogLevel level, const T& first, const Args&... rest) {
        write(first);
        log_helper(level, rest...);
    }
#pragma endregion

private:
    Logger() : logLevel(LogLevel::INFO) {}
};
