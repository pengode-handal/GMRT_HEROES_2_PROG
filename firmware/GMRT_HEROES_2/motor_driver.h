#pragma once

#include <Arduino.h>

class MotorDriver {
public:
    MotorDriver(
        uint8_t leftIn1,
        uint8_t leftIn2,
        uint8_t rightIn1,
        uint8_t rightIn2
    );

    void begin();
    void setReverse(bool leftReversed, bool rightReversed);
    void drive(int leftPwm, int rightPwm);
    void brake();
    void stop();

    int leftPwm() const;
    int rightPwm() const;

private:
    uint8_t _leftIn1;
    uint8_t _leftIn2;
    uint8_t _rightIn1;
    uint8_t _rightIn2;
    bool _leftReversed = false;
    bool _rightReversed = false;
    int _leftPwm = 0;
    int _rightPwm = 0;

    void writeMotor(uint8_t in1, uint8_t in2, int pwm);
    void writeBrake(uint8_t in1, uint8_t in2);
};
