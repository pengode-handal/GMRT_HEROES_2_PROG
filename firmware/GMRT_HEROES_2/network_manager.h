#pragma once

#include <Arduino.h>
#include <WiFi.h>

class NetworkManager {
public:
    void begin();
    bool start(uint32_t nowMs);
    void stop();
    void update(uint32_t nowMs);

    bool active() const;
    bool connected() const;
    bool configured() const;
    bool mdnsReady() const;
    IPAddress localIp() const;

private:
    bool _active = false;
    bool _wasConnected = false;
    bool _configured = false;
    bool _mdnsStarted = false;
    bool _mdnsReady = false;
    uint32_t _lastAttemptMs = 0;
    uint32_t _lastMdnsAttemptMs = 0;

    void startConnection(uint32_t nowMs);
    void startMdns(uint32_t nowMs);
    void stopMdns();
};
