#include <Arduino.h>
#include <WiFi.h>

const char* ssid     = "";
const char* password = "";

IPAddress connectWifi() {
    // Connect to Wi-Fi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to wifi...");
    }
    
    return WiFi.localIP();
}
