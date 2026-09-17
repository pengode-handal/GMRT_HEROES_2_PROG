#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>

#include "robot_types.h"

class UdpReceiver {
public:
    bool begin();
    void stop();
    void update(uint32_t nowMs);

    bool listening() const;
    bool hasFreshTarget(uint32_t nowMs) const;
    const PosePacket& latestPose() const;
    const char* lastPacketStatus() const;

private:
    WiFiUDP _udp;
    PosePacket _pose;
    bool _listening = false;
    bool _hasSequence = false;
    uint32_t _lastSequence = 0;
    const char* _lastPacketStatus = "NO_PACKET";

    bool acceptSequence(uint32_t sequence, uint32_t nowMs);
    void invalidatePose(const char* status, uint32_t nowMs);
    void processPacket(char* packet, uint32_t nowMs);
};

