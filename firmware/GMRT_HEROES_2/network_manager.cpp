#include "network_manager.h"

#include <cstring>
#include <ESPmDNS.h>

#include "config.h"

void NetworkManager::begin() {
    _configured = std::strlen(NetworkConfig::SSID) > 0;

    if (!_configured) {
        Serial.println("Wi-Fi is not configured. Copy secrets.example.h to secrets.h.");
        return;
    }

    Serial.println("Wi-Fi configured. It will start in Autonomous mode.");
}

bool NetworkManager::start(uint32_t nowMs) {
    if (!_configured) {
        Serial.println("Cannot start Wi-Fi because credentials are not configured.");
        return false;
    }

    if (_active) {
        return true;
    }

    _wasConnected = false;
    _mdnsStarted = false;
    _mdnsReady = false;
    _lastAttemptMs = 0;
    _lastMdnsAttemptMs = 0;

    if (!WiFi.mode(WIFI_STA)) {
        Serial.println("Failed to start Wi-Fi station mode.");
        return false;
    }

    // Arduino-ESP32 maps true to WIFI_PS_MIN_MODEM. This is required while
    // Wi-Fi shares the ESP32 radio with Bluepad32 Bluetooth.
    /*if (!WiFi.setSleep(true)) {
        Serial.println("Failed to enable Wi-Fi minimum modem sleep.");
        WiFi.mode(WIFI_OFF);
        return false;
    }*/

    WiFi.setHostname(NetworkConfig::HOSTNAME);
    WiFi.setAutoReconnect(true);
    _active = true;

    btStop();
    startConnection(nowMs);
    return true;
}

void NetworkManager::stop() {
    if (!_active) {
        return;
    }

    stopMdns();
    WiFi.setAutoReconnect(false);
    WiFi.disconnect(true, false);
    WiFi.mode(WIFI_OFF);

    _active = false;
    _wasConnected = false;
    _lastAttemptMs = 0;
    _lastMdnsAttemptMs = 0;

    Serial.println("Wi-Fi stopped. Bluetooth manual mode remains active.");
}

void NetworkManager::update(uint32_t nowMs) {
    if (!_configured || !_active) {
        return;
    }

    bool isConnected = WiFi.status() == WL_CONNECTED;

    if (isConnected && !_wasConnected) {
        Serial.print("Wi-Fi connected. ESP32 IP: ");
        Serial.println(WiFi.localIP());
        startMdns(nowMs);
    } else if (!isConnected && _wasConnected) {
        Serial.println("Wi-Fi connection lost.");
        stopMdns();
    }

    _wasConnected = isConnected;

    if (!isConnected && nowMs - _lastAttemptMs >= NetworkConfig::RETRY_INTERVAL_MS) {
        startConnection(nowMs);
    }

    if (
        isConnected
        && !_mdnsReady
        && nowMs - _lastMdnsAttemptMs >= NetworkConfig::MDNS_RETRY_INTERVAL_MS
    ) {
        startMdns(nowMs);
    }
}

bool NetworkManager::active() const {
    return _active;
}

bool NetworkManager::connected() const {
    return _active && WiFi.status() == WL_CONNECTED;
}

bool NetworkManager::configured() const {
    return _configured;
}

bool NetworkManager::mdnsReady() const {
    return _mdnsReady;
}

IPAddress NetworkManager::localIp() const {
    return WiFi.localIP();
}

void NetworkManager::startConnection(uint32_t nowMs) {
    _lastAttemptMs = nowMs;
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(NetworkConfig::SSID);
    WiFi.begin(NetworkConfig::SSID, NetworkConfig::PASSWORD);
}

void NetworkManager::startMdns(uint32_t nowMs) {
    _lastMdnsAttemptMs = nowMs;
    stopMdns();

    if (!MDNS.begin(NetworkConfig::HOSTNAME)) {
        _mdnsReady = false;
        Serial.println("Failed to start mDNS responder.");
        return;
    }

    _mdnsStarted = true;
    MDNS.setInstanceName(NetworkConfig::HOSTNAME);
    bool serviceAdded = MDNS.addService(
        NetworkConfig::MDNS_SERVICE,
        NetworkConfig::MDNS_PROTOCOL,
        UdpConfig::PORT
    );

    _mdnsReady = serviceAdded;
    if (_mdnsReady) {
        Serial.print("mDNS ready: ");
        Serial.print(NetworkConfig::HOSTNAME);
        Serial.println(".local");
    } else {
        Serial.println("mDNS started, but UDP service registration failed.");
        stopMdns();
    }
}

void NetworkManager::stopMdns() {
    if (_mdnsStarted) {
        MDNS.end();
    }
    _mdnsStarted = false;
    _mdnsReady = false;
}
