#include "robot_controller.h"

#include <cmath>

#include "config.h"

RobotController::RobotController(
    MotorDriver& motors,
    ServoController& servos,
    BluepadInput& gamepad,
    NetworkManager& network,
    UdpReceiver& udp,
    AutonomousController& autonomous
)
    : _motors(motors),
      _servos(servos),
      _gamepad(gamepad),
      _network(network),
      _udp(udp),
      _autonomous(autonomous) {
}

void RobotController::begin() {
    _mode = SystemConfig::START_IN_AUTONOMOUS
        ? RobotMode::AUTONOMOUS
        : RobotMode::MANUAL;

    _servos.begin();
    _motors.begin();
    _motors.setReverse(MotorConfig::LEFT_REVERSED, MotorConfig::RIGHT_REVERSED);
    _gamepad.begin();
    _network.begin();

    if (_mode == RobotMode::AUTONOMOUS) {
        _network.start(millis());
    }

    pinMode(Pins::BUTTON_PIN, INPUT);

    Serial.println();
    Serial.println("GMRT HEROES 2 controller ready.");
    Serial.print("Runtime: ");
    Serial.println(SystemConfig::SIMULATION ? "SIMULATION" : "HARDWARE");
    Serial.print("Mode: ");
    Serial.println(robotModeName(_mode));
}

void RobotController::update() {
    uint32_t nowMs = millis();

    _gamepad.update();
    handleButtons(nowMs);
    updateNetwork(nowMs);
    _udp.update(nowMs);

    if (_emergencyStop) {
        _motors.stop();
    } else if (_mode == RobotMode::MANUAL) {
        updateManual(nowMs);
    } else {
        updateAutonomous(nowMs);
    }

    int buttonOutput = digitalRead(Pins::BUTTON_PIN);
    if (buttonOutput == 0 && _mode != RobotMode::MANUAL) {
        setMode(RobotMode::MANUAL, nowMs);
    } else if (buttonOutput == 1 && _mode != RobotMode::AUTONOMOUS) {
        setMode(RobotMode::AUTONOMOUS, nowMs);
    }

    printStatus(nowMs);
}

void RobotController::handleButtons(uint32_t nowMs) {
    if (_gamepad.crossPressed()) {
        _emergencyStop = true;
        _motors.brake();
        Serial.println("Emergency stop latched. Press Circle to clear it.");
    }

    if (_gamepad.circlePressed()) {
        _emergencyStop = false;
        _motors.brake();
        Serial.println("Emergency stop cleared.");
    }

    /*if (_gamepad.trianglePressed()) {
        RobotMode nextMode = _mode == RobotMode::MANUAL
            ? RobotMode::AUTONOMOUS
            : RobotMode::MANUAL;
        setMode(nextMode, nowMs);
    }*/
}

void RobotController::setMode(RobotMode nextMode, uint32_t nowMs) {
    if (nextMode == _mode) {
        return;
    }

    // Every radio/control transition starts from a safe motor state.
    _motors.brake();
    _udp.stop();
    _mode = nextMode;

    if (_mode == RobotMode::AUTONOMOUS) {
        Serial.println("Mode changed to AUTONOMOUS. Starting Wi-Fi...");

        if (!_network.start(nowMs)) {
            Serial.println("Wi-Fi start failed. Motors remain braked.");
        }
    } else {
        _network.stop();
        _gamepad.begin();
        Serial.println("Mode changed to MANUAL. Wi-Fi is off.");
    }
}

void RobotController::updateManual(uint32_t nowMs) {
    _autoState = AutonomousState::STOP_NO_POSE;

    if (!_gamepad.connected()) {
        _motors.brake();
        return;
    }

    int throttle = applyDeadzone(
        _gamepad.leftY() * GamepadConfig::THROTTLE_SIGN
    );
    int steering = applyDeadzone(
        _gamepad.leftX() * GamepadConfig::STEERING_SIGN
    );

    int leftMix = throttle + steering;
    int rightMix = throttle - steering;
    int leftMagnitude = abs(leftMix);
    int rightMagnitude = abs(rightMix);
    int peak = leftMagnitude > rightMagnitude ? leftMagnitude : rightMagnitude;
    int normalizer = peak > GamepadConfig::AXIS_LIMIT
        ? peak
        : GamepadConfig::AXIS_LIMIT;

    int leftPwm = static_cast<int>(std::lround(
        static_cast<float>(leftMix) * MotorConfig::MANUAL_PWM_LIMIT / normalizer
    ));
    int rightPwm = static_cast<int>(std::lround(
        static_cast<float>(rightMix) * MotorConfig::MANUAL_PWM_LIMIT / normalizer
    ));

    int brakeValue = constrain(
        _gamepad.brakeTrigger(),
        0,
        GamepadConfig::TRIGGER_MAX
    );

    if (brakeValue >= GamepadConfig::FULL_BRAKE_THRESHOLD) {
        _motors.brake();
    } else {
        float driveScale = static_cast<float>(
            GamepadConfig::TRIGGER_MAX - brakeValue
        ) / GamepadConfig::TRIGGER_MAX;

        leftPwm = static_cast<int>(std::lround(leftPwm * driveScale));
        rightPwm = static_cast<int>(std::lround(rightPwm * driveScale));
        _motors.drive(leftPwm, rightPwm);
    }

    _servos.updateManual(
        _gamepad.rightX(),
        _gamepad.rightY(),
        _gamepad.gripThrottle(),
        nowMs
    );
}

void RobotController::updateAutonomous(uint32_t nowMs) {
    if (!_network.connected() || !_udp.hasFreshTarget(nowMs)) {
        _autoState = AutonomousState::STOP_NO_POSE;
        _motors.brake();
        return;
    }

    const PosePacket& pose = _udp.latestPose();
    MotorCommand command = _autonomous.calculate(pose.tx, pose.tz);
    _autoState = command.state;
    _motors.drive(command.leftPwm, command.rightPwm);
}

void RobotController::updateNetwork(uint32_t nowMs) {
    if (_mode != RobotMode::AUTONOMOUS) {
        return;
    }

    _network.update(nowMs);

    if (_network.connected() && !_udp.listening()) {
        _udp.begin();
    } else if (!_network.connected() && _udp.listening()) {
        _udp.stop();
    }
}

void RobotController::printStatus(uint32_t nowMs) {
    if (nowMs - _lastStatusMs < SystemConfig::STATUS_INTERVAL_MS) {
        return;
    }

    _lastStatusMs = nowMs;

    Serial.printf(
        "mode=%s estop=%d gamepad=%d wifi_active=%d wifi=%d mdns=%d pwm_left=%d pwm_right=%d",
        robotModeName(_mode),
        _emergencyStop,
        _gamepad.connected(),
        _network.active(),
        _network.connected(),
        _network.mdnsReady(),
        _motors.leftPwm(),
        _motors.rightPwm()
    );

    if (_mode == RobotMode::AUTONOMOUS) {
        const PosePacket& pose = _udp.latestPose();
        Serial.printf(
            " auto=%s udp=%s marker=%d tx=%.3f tz=%.3f",
            autonomousStateName(_autoState),
            _udp.lastPacketStatus(),
            pose.markerId,
            pose.tx,
            pose.tz
        );

    } else {
        Serial.printf(
            " grip=%d arm=%d wrist=%d brake=%d",
            _servos.gripAngle(),
            _servos.armAngle(),
            _servos.wristAngle(),
            _gamepad.brakeTrigger()
        );
    }

    if (SystemConfig::SIMULATION) {
        Serial.print(" simulation=1");
    }

    Serial.println();
}

int RobotController::applyDeadzone(int value) const {
    return abs(value) < GamepadConfig::DRIVE_DEADZONE ? 0 : value;
}
