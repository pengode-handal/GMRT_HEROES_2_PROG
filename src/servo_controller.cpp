#include "servo_controller.h"
#include "config.h"

ServoController::ServoController(
    uint8_t servo1Pin,
    uint8_t servo2Pin
)
    : _servo1Pin(servo1Pin),
      _servo2Pin(servo2Pin) {
}

void ServoController::begin() {

    _servo1.attach(_servo1Pin);
    _servo2.attach(_servo2Pin);

    // Posisi awal
    _servo1.write(ServoConfig::INITIAL_ANGLE);
    _servo2.write(ServoConfig::INITIAL_ANGLE);
}

void ServoController::openGrip() {

    _servo1.write(ServoConfig::GRIP_OPEN_ANGLE);
    _servo2.write(ServoConfig::GRIP_OPEN_ANGLE);
}

void ServoController::closeGrip() {

    _servo1.write(ServoConfig::GRIP_CLOSE_ANGLE);
    _servo2.write(ServoConfig::GRIP_CLOSE_ANGLE);
}