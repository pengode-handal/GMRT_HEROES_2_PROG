#include "udp_receiver.h"

#include <cmath>
#include <cstdio>

#include "config.h"

bool UdpReceiver::begin() {
    if (_listening) {
        return true;
    }

    _listening = _udp.begin(UdpConfig::PORT) == 1;

    if (_listening) {
        Serial.print("UDP receiver listening on port ");
        Serial.println(UdpConfig::PORT);
    } else {
        Serial.println("Failed to start UDP receiver.");
    }

    return _listening;
}

void UdpReceiver::stop() {
    if (_listening) {
        _udp.stop();
    }

    _listening = false;
    _hasSequence = false;
    _pose.markerVisible = false;
    _lastPacketStatus = "UDP_STOPPED";
}

void UdpReceiver::update(uint32_t nowMs) {
    if (!_listening) {
        return;
    }

    int packetSize = 0;

    while ((packetSize = _udp.parsePacket()) > 0) {
        if (packetSize >= static_cast<int>(UdpConfig::BUFFER_SIZE)) {
            while (_udp.available()) {
                _udp.read();
            }
            invalidatePose("PACKET_TOO_LARGE", nowMs);
            continue;
        }

        char packet[UdpConfig::BUFFER_SIZE];
        int length = _udp.read(packet, sizeof(packet) - 1);

        if (length <= 0) {
            invalidatePose("EMPTY_PACKET", nowMs);
            continue;
        }

        packet[length] = '\0';
        processPacket(packet, nowMs);
    }
}

bool UdpReceiver::listening() const {
    return _listening;
}

bool UdpReceiver::hasFreshTarget(uint32_t nowMs) const {
    return _pose.markerVisible
        && _pose.markerId == UdpConfig::TARGET_MARKER_ID
        && nowMs - _pose.receivedAtMs <= UdpConfig::POSE_TIMEOUT_MS;
}

const PosePacket& UdpReceiver::latestPose() const {
    return _pose;
}

const char* UdpReceiver::lastPacketStatus() const {
    return _lastPacketStatus;
}

bool UdpReceiver::acceptSequence(uint32_t sequence, uint32_t nowMs) {
    if (!_hasSequence || nowMs - _pose.receivedAtMs > UdpConfig::POSE_TIMEOUT_MS) {
        _hasSequence = true;
        _lastSequence = sequence;
        return true;
    }

    int32_t difference = static_cast<int32_t>(sequence - _lastSequence);

    if (difference <= 0) {
        _lastPacketStatus = "OLD_SEQUENCE";
        return false;
    }

    _lastSequence = sequence;
    return true;
}

void UdpReceiver::invalidatePose(const char* status, uint32_t nowMs) {
    _pose.markerVisible = false;
    _pose.receivedAtMs = nowMs;
    _lastPacketStatus = status;
}

void UdpReceiver::processPacket(char* packet, uint32_t nowMs) {
    if (packet[0] == 'P') {
        unsigned long sequence = 0;
        int markerId = -1;
        float tx = 0.0f;
        float tz = 0.0f;

        int parsed = std::sscanf(
            packet,
            "P,%lu,%d,%f,%f",
            &sequence,
            &markerId,
            &tx,
            &tz
        );

        if (parsed != 4) {
            invalidatePose("INVALID_POSE_PACKET", nowMs);
            return;
        }

        if (!acceptSequence(static_cast<uint32_t>(sequence), nowMs)) {
            return;
        }

        if (!std::isfinite(tx) || !std::isfinite(tz) || tz <= 0.0f) {
            invalidatePose("INVALID_POSE_VALUE", nowMs);
            return;
        }

        _pose.sequence = static_cast<uint32_t>(sequence);
        _pose.markerId = markerId;
        _pose.tx = tx;
        _pose.tz = tz;
        _pose.receivedAtMs = nowMs;
        _pose.markerVisible = markerId == UdpConfig::TARGET_MARKER_ID;
        _lastPacketStatus = _pose.markerVisible ? "POSE" : "WRONG_MARKER";
        return;
    }

    if (packet[0] == 'N') {
        unsigned long sequence = 0;

        if (std::sscanf(packet, "N,%lu", &sequence) != 1) {
            invalidatePose("INVALID_NO_MARKER_PACKET", nowMs);
            return;
        }

        if (!acceptSequence(static_cast<uint32_t>(sequence), nowMs)) {
            return;
        }

        _pose.sequence = static_cast<uint32_t>(sequence);
        _pose.markerId = -1;
        _pose.tx = 0.0f;
        _pose.tz = 0.0f;
        _pose.receivedAtMs = nowMs;
        _pose.markerVisible = false;
        _lastPacketStatus = "NO_MARKER";
        return;
    }

    invalidatePose("UNKNOWN_PACKET", nowMs);
}
