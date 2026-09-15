#include <Arduino.h>
#include "esp_bt_device.h"

void setup() {

    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println("ESP32 Bluetooth MAC Address");
    Serial.println("================================");

    const uint8_t* address = esp_bt_dev_get_address();

    if (address == nullptr) {
        Serial.println("Bluetooth MAC tidak tersedia.");
        return;
    }

    Serial.printf(
        "%02X:%02X:%02X:%02X:%02X:%02X\n",
        address[0],
        address[1],
        address[2],
        address[3],
        address[4],
        address[5]
    );

    Serial.println();
    Serial.println("MAC address siap digunakan");
    Serial.println("untuk pairing DualShock 4.");
}

void loop() {
}
