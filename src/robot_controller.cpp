#include "robot_controller.h"
#include "config.h"

RobotController::RobotController(
    MotorDriver& motors,
    ServoController& servos,
    PS4Input& ps4
)
    : _motors(motors),
      _servos(servos),
      _ps4(ps4) {
}

void RobotController::begin() {

    _motors.begin();

    _servos.begin();

    _ps4.begin();

    Serial.println("Robot controller initialized.");
}

int RobotController::applyDeadzone(int value) {

    if (abs(value) < PS4Config::DEADZONE) {
        return 0;
    }

    return value;
}

void RobotController::update() {

    if (!_ps4.connected()) {

        _motors.stop();

        return;
    }

    if (_ps4.cross()) {

        _motors.stop();

        return;
    }

    updateMovement();
    updateGripper();
}

void RobotController::updateMovement() {

    int x = _ps4.leftX();

    // PS4: stick up = negative
    // Robot: forward = positive
    int y = -_ps4.leftY();

    x = applyDeadzone(x);
    y = applyDeadzone(y);

    if (x == 0 && y == 0) {

        _motors.stop();

        return;
    }

    // Differential drive mixing
    int rawLeft = y + x;
    int rawRight = y - x;

    // Convert joystick range to PWM range
    int leftPWM = constrain(
        rawLeft * 2,
        -255,
        255
    );

    int rightPWM = constrain(
        rawRight * 2,
        -255,
        255
    );

    _motors.drive(
        leftPWM,
        rightPWM
    );
}

void RobotController::updateGripper() {

    if (_ps4.r1()) {

        _servos.openGrip();
    }

    if (_ps4.l1()) {

        _servos.closeGrip();
    }
}