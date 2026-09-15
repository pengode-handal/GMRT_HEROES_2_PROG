#include <Arduino.h>
#include <WiFiUdp.h>

#include "wifi.h"
#include "udp.h"

char packetBuffer[255]; // Buffer to hold incoming packet

void setup() {
  Serial.begin(115200);

  delay(2000);

  Serial.println("Starting...");

  IPAddress ip = connectWifi();

  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(ip);

  int localPort = setupUDP();
  Serial.printf("Listening on UDP port %d\n", localPort);
}

int i = 0;

void loop() {
  int packetSize = readUDPPacket(packetBuffer);
  
  if (packetSize) {
    Serial.println(packetSize);

    auto packet = parseUDPPacket(packetBuffer);

    Pose pose;
    if (packetToPose(packet, pose)) {
      Serial.printf("Sequence: %d\n", pose.sequence);

      if (pose.hasMarker) {
        Serial.printf("Marker %d pose: tx=%.3f, ty=%.3f\n",
                      pose.markerId, pose.tx, pose.ty);
      } else {
        Serial.println("No marker pose");
      }
    } else {
      Serial.println("Invalid packet");
    }
  }
}