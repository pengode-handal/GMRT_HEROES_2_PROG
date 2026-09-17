#include "servo_controller.h"

#include "config.h"

ServoController::ServoController(
    uint8_t gripPin,
    uint8_t armPin,
    uint8_t wristPin
)
    : _gripPin(gripPin),
      _armPin(armPin),
      _wristPin(wristPin),
      _gripAngle(ServoConfig::GRIP_INITIAL_DEG),
      _armAngle(ServoConfig::ARM_INITIAL_DEG),
      _wristAngle(ServoConfig::WRIST_INITIAL_DEG) {
}

void ServoController::begin() {
    if (!SystemConfig::SIMULATION) {
        _grip.setPeriodHertz(50);
        _arm.setPeriodHertz(50);
        _wrist.setPeriodHertz(50);

        _grip.attach(
            _gripPin,
            ServoConfig::PULSE_MIN_US,
            ServoConfig::PULSE_MAX_US
        );
        _arm.attach(
            _armPin,
            ServoConfig::PULSE_MIN_US,
            ServoConfig::PULSE_MAX_US
        );
        _wrist.attach(
            _wristPin,
            ServoConfig::PULSE_MIN_US,
            ServoConfig::PULSE_MAX_US
        );
    }

    writeGrip();
    writeArm();
    writeWrist();
}

void ServoController::openGrip() {
    _gripAngle = ServoConfig::GRIP_OPEN_DEG;
    writeGrip();
}

void ServoController::closeGrip() {
    _gripAngle = ServoConfig::GRIP_CLOSED_DEG;
    writeGrip();
}

void ServoController::setGripThrottle(int triggerValue) {
    int limitedTrigger = constrain(
        triggerValue,
        0,
        ServoConfig::GRIP_TRIGGER_MAX
    );
    int newAngle = map(
        limitedTrigger,
        0,
        ServoConfig::GRIP_TRIGGER_MAX,
        ServoConfig::GRIP_OPEN_DEG,
        ServoConfig::GRIP_CLOSED_DEG
    );

    if (newAngle != _gripAngle) {
        _gripAngle = newAngle;
        writeGrip();
    }
}

void ServoController::updateManual(
    int rightX,
    int rightY,
    int gripTrigger,
    uint32_t nowMs
) {
    if (nowMs - _lastUpdateMs < ServoConfig::UPDATE_INTERVAL_MS) {
        return;
    }

    _lastUpdateMs = nowMs;
    setGripThrottle(gripTrigger);
    bool armChanged = false;
    bool wristChanged = false;

    if (rightY > ServoConfig::STICK_DEADZONE) {
        _armAngle += ServoConfig::STEP_DEG;
        armChanged = true;
    } else if (rightY < -ServoConfig::STICK_DEADZONE) {
        _armAngle -= ServoConfig::STEP_DEG;
        armChanged = true;
    }

    if (rightX > ServoConfig::STICK_DEADZONE) {
        _wristAngle += ServoConfig::STEP_DEG;
        wristChanged = true;
    } else if (rightX < -ServoConfig::STICK_DEADZONE) {
        _wristAngle -= ServoConfig::STEP_DEG;
        wristChanged = true;
    }

    _armAngle = constrain(
        _armAngle,
        ServoConfig::ARM_MIN_DEG,
        ServoConfig::ARM_MAX_DEG
    );
    _wristAngle = constrain(
        _wristAngle,
        ServoConfig::WRIST_MIN_DEG,
        ServoConfig::WRIST_MAX_DEG
    );

    if (armChanged) {
        writeArm();
    }
    if (wristChanged) {
        writeWrist();
    }
}

int ServoController::gripAngle() const {
    return _gripAngle;
}

int ServoController::armAngle() const {
    return _armAngle;
}

int ServoController::wristAngle() const {
    return _wristAngle;
}

void ServoController::writeGrip() {
    if (!SystemConfig::SIMULATION) {
        _grip.write(_gripAngle);
    }
}

void ServoController::writeArm() {
    if (!SystemConfig::SIMULATION) {
        _arm.write(_armAngle);
    }
}

void ServoController::writeWrist() {
    if (!SystemConfig::SIMULATION) {
        _wrist.write(_wristAngle);
    }
}
