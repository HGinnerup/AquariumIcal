#pragma once

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "common.h"

void wifiSetup(const char* ssid, const char* password, bool locking = true) {

}

String httpsGet(const String& url, const char* CACertification) {
    std::array<char, 128> buffer;
    String result;

    String command = "curl -s \"" + url + "\"";

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

