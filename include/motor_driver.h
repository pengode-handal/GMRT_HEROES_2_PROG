#pragma once

#include <Arduino.h>

class MotorDriver {
public:
    MotorDriver(
        uint8_t ena,
        uint8_t in1,
        uint8_t in2,
        uint8_t in3,
        uint8_t in4,
        uint8_t enb
    );

    void begin();

    void setReverse(bool leftReverse, bool rightReverse);

    void drive(int leftPWM, int rightPWM);

    void stop();

private:
    uint8_t _ena;
    uint8_t _in1;
    uint8_t _in2;

    uint8_t _in3;
    uint8_t _in4;
    uint8_t _enb;

    bool _leftReverse = false;
    bool _rightReverse = false;

    void setLeftMotor(int pwm);
    void setRightMotor(int pwm);
};