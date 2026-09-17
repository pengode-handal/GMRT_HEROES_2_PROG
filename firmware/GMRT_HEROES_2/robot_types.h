#pragma once

#include <Arduino.h>

enum class RobotMode : uint8_t {
    MANUAL,
    AUTONOMOUS,
};

enum class AutonomousState : uint8_t {
    STOP_NO_POSE,
    STOP_INVALID_POSE,
    STOP_TARGET_REACHED,
    ALIGN_LEFT,
    ALIGN_RIGHT,
    APPROACH_LEFT,
    APPROACH_RIGHT,
    APPROACH_STRAIGHT,
};

struct PosePacket {
    uint32_t sequence = 0;
    int markerId = -1;
    float tx = 0.0f;
    float tz = 0.0f;
    uint32_t receivedAtMs = 0;
    bool markerVisible = false;
};

struct MotorCommand {
    int leftPwm = 0;
    int rightPwm = 0;
    int forwardPwm = 0;
    int turnPwm = 0;
    float angleDeg = 0.0f;
    float distanceErrorM = 0.0f;
    AutonomousState state = AutonomousState::STOP_NO_POSE;
};

const char* robotModeName(RobotMode mode);
const char* autonomousStateName(AutonomousState state);
