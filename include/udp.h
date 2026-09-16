#include <WiFiUdp.h>
#include <string>
#include <vector>

struct Pose {
	int sequence;
	bool hasMarker;
	int markerId;
	float tx;
	float ty;
};

WiFiUDP getUDP();
int setupUDP();
int readUDPPacket(char* packetBuffer);
std::vector<std::string> parseUDPPacket(const char* buffer);
bool packetToPose(const std::vector<std::string>& packet, Pose& pose);
