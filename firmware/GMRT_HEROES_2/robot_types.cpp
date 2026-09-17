#include "robot_types.h"

const char* robotModeName(RobotMode mode) {
    switch (mode) {
        case RobotMode::MANUAL:
            return "MANUAL";
        case RobotMode::AUTONOMOUS:
            return "AUTONOMOUS";
        default:
            return "UNKNOWN";
    }
}

const char* autonomousStateName(AutonomousState state) {
    switch (state) {
        case AutonomousState::STOP_NO_POSE:
            return "STOP_NO_POSE";
        case AutonomousState::STOP_INVALID_POSE:
            return "STOP_INVALID_POSE";
        case AutonomousState::STOP_TARGET_REACHED:
            return "STOP_TARGET_REACHED";
        case AutonomousState::ALIGN_LEFT:
            return "ALIGN_LEFT";
        case AutonomousState::ALIGN_RIGHT:
            return "ALIGN_RIGHT";
        case AutonomousState::APPROACH_LEFT:
            return "APPROACH_LEFT";
        case AutonomousState::APPROACH_RIGHT:
            return "APPROACH_RIGHT";
        case AutonomousState::APPROACH_STRAIGHT:
            return "APPROACH_STRAIGHT";
        default:
            return "UNKNOWN";
    }
}
