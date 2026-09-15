#include <WiFiUdp.h>
#include <sstream>
#include <vector>
#include <string>

#include "udp.h"

unsigned int localPort = 1337; // Port to listen on

WiFiUDP udp;

WiFiUDP getUDP() {
    return udp;
}

int setupUDP() {\
    // Begin listening on local UDP port
    udp.begin(localPort);

    return localPort;
}

int readUDPPacket(char* packetBuffer) {
    int packetSize = udp.parsePacket();

    if (packetSize > 0) {
        // Read packet into buffer
        int len = udp.read(packetBuffer, 255);
        if (len > 0) {
          packetBuffer[len] = 0; // Null-terminate string
        }

        return packetSize;
    }

    return 0;
}

std::vector<std::string> parseUDPPacket(const char* buffer) {
  std::vector<std::string> result;
  if (buffer == nullptr) return result;

  std::stringstream ss(buffer);
  std::string token;

  while (std::getline(ss, token, ',')) {
    if (!token.empty()) {
        result.push_back(token);
    }
  }

  return result;
}

bool packetToPose(const std::vector<std::string>& packet, Pose& pose) {
  try {
    if (packet.size() == 2 && packet[0] == "N") {
      pose.sequence = std::stoi(packet[1]);
      pose.hasMarker = false;
      pose.markerId = 0;
      pose.tx = 0.0f;
      pose.ty = 0.0f;
      return true;
    }

    if (packet.size() == 5 && packet[0] == "P") {
      pose.sequence = std::stoi(packet[1]);
      pose.hasMarker = true;
      pose.markerId = std::stoi(packet[2]);
      pose.tx = std::stof(packet[3]);
      pose.ty = std::stof(packet[4]);
      return true;
    }
  } catch (const std::exception&) {
    return false;
  }

  return false;
}
