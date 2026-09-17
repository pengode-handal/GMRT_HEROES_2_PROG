#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>

const char* WIFI_SSID = "SM-M146B";
const char* WIFI_PASSWORD = "pahlevi2007";
const char* HOSTNAME = "gmrt-heroes-2";

const char* statusName(wl_status_t status) {
    switch (status) {
        case WL_IDLE_STATUS:
            return "IDLE";
        case WL_NO_SSID_AVAIL:
            return "SSID_NOT_FOUND";
        case WL_SCAN_COMPLETED:
            return "SCAN_COMPLETED";
        case WL_CONNECTED:
            return "CONNECTED";
        case WL_CONNECT_FAILED:
            return "AUTH_FAILED";
        case WL_CONNECTION_LOST:
            return "CONNECTION_LOST";
        case WL_DISCONNECTED:
            return "DISCONNECTED";
        default:
            return "UNKNOWN";
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA);

    /*if (!WiFi.setSleep(true)) {
        Serial.println("Failed to enable WIFI_PS_MIN_MODEM.");
        return;
    }*/

    Serial.println("Wi-Fi power save: WIFI_PS_MIN_MODEM.");
    WiFi.setHostname(HOSTNAME);
    WiFi.disconnect();
    delay(300);

    Serial.println("Scanning 2.4 GHz Wi-Fi networks...");
    int count = WiFi.scanNetworks();

    for (int index = 0; index < count; index++) {
        Serial.printf(
            "%d. %s | RSSI=%d dBm | channel=%d\n",
            index + 1,
            WiFi.SSID(index).c_str(),
            WiFi.RSSI(index),
            WiFi.channel(index)
        );
    }

    WiFi.scanDelete();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    uint32_t startedAt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startedAt < 20000) {
        Serial.printf("status=%d (%s)\n", WiFi.status(), statusName(WiFi.status()));
        delay(1000);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Connected. ESP32 IP: ");
        Serial.println(WiFi.localIP());

        if (MDNS.begin(HOSTNAME)) {
            MDNS.setInstanceName(HOSTNAME);
            MDNS.addService("gmrt-udp", "udp", 4210);
            Serial.print("mDNS ready: ");
            Serial.print(HOSTNAME);
            Serial.println(".local");
        } else {
            Serial.println("mDNS failed to start.");
        }
    } else {
        Serial.print("Connection failed. Final status: ");
        Serial.println(statusName(WiFi.status()));
    }
}

void loop() {
    delay(1000);
}
