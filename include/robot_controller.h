#pragma once

#include "motor_driver.h"
#include "servo_controller.h"
#include "ps4_input.h"

class RobotController {
public:
    RobotController(
        MotorDriver& motors,
        ServoController& servos,
        PS4Input& ps4
    );

    void begin();
    void update();

private:
    MotorDriver& _motors;
    ServoController& _servos;
    PS4Input& _ps4;

    void updateMovement();
    void updateGripper();

    int applyDeadzone(int value);
};