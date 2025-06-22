#pragma once

#include <WiFi.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>


void wifiSetup(const char* ssid, const char* password, bool locking=true) {
    // First step is to configure WiFi STA and connect in order to get the current time and date.
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);

    if(locking) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println(" CONNECTED");
    }
}



String httpsGet(const char* url, const char* CACertification, bool verbose=false) {

    String output = String();

    NetworkClientSecure *client = new NetworkClientSecure;
    if (!client) {
        Serial.println("Unable to create client");
        throw "Unable to create client";
    }

    client->setCACert(CACertification);
    { // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
        
        HTTPClient https;

        if (verbose) Serial.print("[HTTPS] begin...\n");
        if (!https.begin(*client, url)) {  // HTTPS
            Serial.println("[HTTPS] Unable to connect");
            throw "[HTTPS] Unable to connect\n";
        }


        if (verbose) Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode <= 0) {
            Serial.print("[HTTPS] GET... failed, error: ");
            Serial.println(https.errorToString(httpCode).c_str());
            throw String("[HTTPS] GET... failed, error: ") + https.errorToString(httpCode).c_str();
        }


        // HTTP header has been send and Server response header has been handled
        if (verbose) Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            output = https.getString();
        }

        https.end();

    } // End extra scoping block
    
    delete client;

    return output;
}

