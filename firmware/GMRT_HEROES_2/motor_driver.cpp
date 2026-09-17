#include "motor_driver.h"

#include "config.h"

MotorDriver::MotorDriver(
    uint8_t leftIn1,
    uint8_t leftIn2,
    uint8_t rightIn1,
    uint8_t rightIn2
)
    : _leftIn1(leftIn1),
      _leftIn2(leftIn2),
      _rightIn1(rightIn1),
      _rightIn2(rightIn2) {
}

void MotorDriver::begin() {
    if (!SystemConfig::SIMULATION) {
        pinMode(_leftIn1, OUTPUT);
        pinMode(_leftIn2, OUTPUT);
        pinMode(_rightIn1, OUTPUT);
        pinMode(_rightIn2, OUTPUT);
    }

    stop();
}

void MotorDriver::setReverse(bool leftReversed, bool rightReversed) {
    _leftReversed = leftReversed;
    _rightReversed = rightReversed;
}

void MotorDriver::drive(int leftPwm, int rightPwm) {
    _leftPwm = -constrain(leftPwm, -MotorConfig::PWM_LIMIT, MotorConfig::PWM_LIMIT);
    _rightPwm = -constrain(rightPwm, -MotorConfig::PWM_LIMIT, MotorConfig::PWM_LIMIT);

    int leftOutput = _leftReversed ? -_leftPwm : _leftPwm;
    int rightOutput = _rightReversed ? -_rightPwm : _rightPwm;

    writeMotor(_leftIn1, _leftIn2, leftOutput);
    writeMotor(_rightIn1, _rightIn2, rightOutput);
}

void MotorDriver::stop() {
    brake();
}

void MotorDriver::brake() {
    _leftPwm = 0;
    _rightPwm = 0;
    writeBrake(_leftIn1, _leftIn2);
    writeBrake(_rightIn1, _rightIn2);
}

int MotorDriver::leftPwm() const {
    return _leftPwm;
}

int MotorDriver::rightPwm() const {
    return _rightPwm;
}

void MotorDriver::writeMotor(uint8_t in1, uint8_t in2, int pwm) {
    if (SystemConfig::SIMULATION) {
        return;
    }

    int speed = constrain(abs(pwm), 0, MotorConfig::PWM_LIMIT);

    analogWrite(in1, 0);
    analogWrite(in2, 0);

    if (pwm > 0) {
        analogWrite(in1, speed);
    } else if (pwm < 0) {
        analogWrite(in2, speed);
    }
}

void MotorDriver::writeBrake(uint8_t in1, uint8_t in2) {
    if (SystemConfig::SIMULATION) {
        return;
    }

    // Equal L298N inputs produce dynamic braking while the bridge is enabled.
    analogWrite(in1, 0);
    analogWrite(in2, 0);
}
