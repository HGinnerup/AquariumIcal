#pragma once

#include <AsyncTCP.h>
#include <Print.h>


template <uint16_t port = 23> // Template to have static methods, per port, to use for TCP callbacks
class TelnetServer : public Print {
    AsyncServer* server;
    std::vector<AsyncClient*> clients;

#pragma region TCP Callbacks
protected:
    static void onClient(void* arg, AsyncClient* client) {
        getInstance()->clients.emplace_back(client);
    }

#pragma endregion

#pragma region Singleton
public:
    static TelnetServer<port>* getInstance() {
        static TelnetServer<port> instance;
        return &instance;
    }

protected:
    TelnetServer() {
        server = new AsyncServer(port);

        server->onClient(TelnetServer<port>::onClient, nullptr);
        server->setNoDelay(false); // Send immediately vs build packets
    }
#pragma endregion

#pragma region Status
public:
int getClientCount() {
    int count = 0;
    for (auto client : this->clients) {
        if (client->connected()) {
            count++;
        }
    }
    return count;
}
#pragma endregion

#pragma region TCP management
public:
    void begin() {
        server->begin();
    }

    void end() {
        server->end();
    }


    uint8_t getStatus() {
        return server->status();
    }
#pragma endregion

#pragma region write
    size_t write(uint8_t ch) override {
        for (auto client : this->clients) {
            if (client->connected()) {
                client->write((char*)(&ch), 1);
                client->send();
            }
        }
        return 1;
    }

    size_t write(const uint8_t* buffer, const size_t size) override {
        for (auto client : this->clients) {
            if (client->connected()) {
                client->write(reinterpret_cast<const char*>(buffer), size);
                client->send();
            }
        }
        return size;
    }

    void flush() override {
        for (auto client : this->clients) {
            if (client->connected()) {
                client->send();
            }
        }
    }

#pragma endregion

};
