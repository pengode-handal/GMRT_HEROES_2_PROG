#include "motor_driver.h"

MotorDriver::MotorDriver(
    uint8_t ena,
    uint8_t in1,
    uint8_t in2,
    uint8_t in3,
    uint8_t in4,
    uint8_t enb
)
    : _ena(ena),
      _in1(in1),
      _in2(in2),
      _in3(in3),
      _in4(in4),
      _enb(enb) {
}

void MotorDriver::begin() {

    pinMode(_ena, OUTPUT);

    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);

    pinMode(_in3, OUTPUT);
    pinMode(_in4, OUTPUT);

    pinMode(_enb, OUTPUT);

    stop();
}

void MotorDriver::setReverse(
    bool leftReverse,
    bool rightReverse
) {
    _leftReverse = leftReverse;
    _rightReverse = rightReverse;
}

void MotorDriver::drive(
    int leftPWM,
    int rightPWM
) {
    leftPWM = constrain(leftPWM, -255, 255);
    rightPWM = constrain(rightPWM, -255, 255);

    setLeftMotor(leftPWM);
    setRightMotor(rightPWM);
}

void MotorDriver::setLeftMotor(int pwm) {

    if (_leftReverse) {
        pwm = -pwm;
    }

    int speed = abs(pwm);
    speed = constrain(speed, 0, 255);

    if (pwm > 0) {
        digitalWrite(_in1, HIGH);
        digitalWrite(_in2, LOW);
    }
    else if (pwm < 0) {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, HIGH);
    }
    else {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, LOW);
    }

    analogWrite(_ena, speed);
}

void MotorDriver::setRightMotor(int pwm) {

    if (_rightReverse) {
        pwm = -pwm;
    }

    int speed = abs(pwm);
    speed = constrain(speed, 0, 255);

    if (pwm > 0) {
        digitalWrite(_in3, HIGH);
        digitalWrite(_in4, LOW);
    }
    else if (pwm < 0) {
        digitalWrite(_in3, LOW);
        digitalWrite(_in4, HIGH);
    }
    else {
        digitalWrite(_in3, LOW);
        digitalWrite(_in4, LOW);
    }

    analogWrite(_enb, speed);
}

void MotorDriver::stop() {

    digitalWrite(_in1, LOW);
    digitalWrite(_in2, LOW);

    digitalWrite(_in3, LOW);
    digitalWrite(_in4, LOW);

    analogWrite(_ena, 0);
    analogWrite(_enb, 0);
}