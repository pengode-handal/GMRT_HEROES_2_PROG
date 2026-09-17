#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoController {
public:
    ServoController(uint8_t gripPin, uint8_t armPin, uint8_t wristPin);

    void begin();
    void openGrip();
    void closeGrip();
    void setGripThrottle(int triggerValue);
    void updateManual(
        int rightX,
        int rightY,
        int gripTrigger,
        uint32_t nowMs
    );

    int gripAngle() const;
    int armAngle() const;
    int wristAngle() const;

private:
    Servo _grip;
    Servo _arm;
    Servo _wrist;
    uint8_t _gripPin;
    uint8_t _armPin;
    uint8_t _wristPin;
    int _gripAngle;
    int _armAngle;
    int _wristAngle;
    uint32_t _lastUpdateMs = 0;

    void writeGrip();
    void writeArm();
    void writeWrist();
};
