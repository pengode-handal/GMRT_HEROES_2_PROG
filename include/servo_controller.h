#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoController {
public:
    ServoController(
        uint8_t servo1Pin,
        uint8_t servo2Pin
    );

    void begin();

    void openGrip();
    void closeGrip();

private:
    Servo _servo1;
    Servo _servo2;

    uint8_t _servo1Pin;
    uint8_t _servo2Pin;
};