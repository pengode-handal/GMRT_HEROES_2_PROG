#include <PS4Controller.h>
#include "esp_bt_device.h"

void setup() {
  Serial.begin(115200);

  PS4.begin();

  const uint8_t* address = esp_bt_dev_get_address();

  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32 PS4 Controller Pairing");
  Serial.println("================================");

  Serial.printf(
    "Bluetooth MAC ESP32: %02X:%02X:%02X:%02X:%02X:%02X\n",
    address[0],
    address[1],
    address[2],
    address[3],
    address[4],
    address[5]
  );

  Serial.println();
  Serial.println("Masukkan MAC address di atas");
  Serial.println("ke controller PS4 menggunakan");
  Serial.println("SixaxisPairTool.");
  Serial.println();
  Serial.println("Setelah itu tekan tombol PS.");
}

void loop() {
  if (PS4.isConnected()) {
    Serial.println("PS4 CONTROLLER TERHUBUNG!");
    delay(1000);
  }
}