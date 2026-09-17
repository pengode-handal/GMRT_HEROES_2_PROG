#include "autonomous_controller.h"

#include <Arduino.h>
#include <cmath>

#include "config.h"

MotorCommand AutonomousController::calculate(float tx, float tz) const {
    MotorCommand command;

    tx *= AutonomousConfig::CAMERA_X_SIGN;

    if (!std::isfinite(tx) || !std::isfinite(tz) || tz <= 0.0f) {
        command.state = AutonomousState::STOP_INVALID_POSE;
        return command;
    }

    command.distanceErrorM = tz - AutonomousConfig::STOP_DISTANCE_M;

    if (command.distanceErrorM <= 0.0f) {
        command.state = AutonomousState::STOP_TARGET_REACHED;
        return command;
    }

    command.angleDeg = std::atan2(tx, tz) * 180.0f / PI;
    command.forwardPwm = static_cast<int>(std::lround(
        AutonomousConfig::DISTANCE_KP * command.distanceErrorM
    ));
    command.forwardPwm = constrain(
        command.forwardPwm,
        0,
        AutonomousConfig::MAX_FORWARD_PWM
    );

    command.turnPwm = static_cast<int>(std::lround(
        AutonomousConfig::TURN_KP * command.angleDeg
    ));
    command.turnPwm = constrain(
        command.turnPwm,
        -AutonomousConfig::MAX_TURN_PWM,
        AutonomousConfig::MAX_TURN_PWM
    );

    if (std::fabs(command.angleDeg) > AutonomousConfig::ALIGN_ONLY_ANGLE_DEG) {
        command.forwardPwm = 0;
        command.state = command.angleDeg > 0.0f
            ? AutonomousState::ALIGN_RIGHT
            : AutonomousState::ALIGN_LEFT;
    } else if (command.angleDeg > AutonomousConfig::STRAIGHT_DEADBAND_DEG) {
        command.state = AutonomousState::APPROACH_RIGHT;
    } else if (command.angleDeg < -AutonomousConfig::STRAIGHT_DEADBAND_DEG) {
        command.state = AutonomousState::APPROACH_LEFT;
    } else {
        command.state = AutonomousState::APPROACH_STRAIGHT;
    }

    command.leftPwm = constrain(
        command.forwardPwm + command.turnPwm,
        -MotorConfig::PWM_LIMIT,
        MotorConfig::PWM_LIMIT
    );
    command.rightPwm = constrain(
        command.forwardPwm - command.turnPwm,
        -MotorConfig::PWM_LIMIT,
        MotorConfig::PWM_LIMIT
    );

    return command;
}

