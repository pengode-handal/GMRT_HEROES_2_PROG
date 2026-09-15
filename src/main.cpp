#include <Arduino.h>

#include "config.h"
#include "motor_driver.h"
#include "servo_controller.h"
#include "ps4_input.h"
#include "robot_controller.h"

MotorDriver motors(
    MotorPins::ENA,
    MotorPins::IN1,
    MotorPins::IN2,
    MotorPins::IN3,
    MotorPins::IN4,
    MotorPins::ENB
);

ServoController servos(
    ServoPins::SERVO_1,
    ServoPins::SERVO_2
);

PS4Input ps4;

RobotController robot(
    motors,
    servos,
    ps4
);

void setup() {

    Serial.begin(115200);

    delay(500);

    Serial.println();
    Serial.println("==============================");
    Serial.println("PS4 ROBOT CONTROLLER");
    Serial.println("==============================");

    // Ubah menjadi true jika arah motor salah
    motors.setReverse(false, false);

    robot.begin();
}

void loop() {

    robot.update();

    delay(20);
}