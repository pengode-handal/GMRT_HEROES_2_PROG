#include <Arduino.h>

#include "autonomous_controller.h"
#include "bluepad_input.h"
#include "config.h"
#include "motor_driver.h"
#include "network_manager.h"
#include "robot_controller.h"
#include "servo_controller.h"
#include "udp_receiver.h"

MotorDriver motors(
    Pins::MOTOR_LEFT_IN1,
    Pins::MOTOR_LEFT_IN2,
    Pins::MOTOR_RIGHT_IN1,
    Pins::MOTOR_RIGHT_IN2
);

ServoController servos(
    Pins::GRIP_SERVO,
    Pins::ARM_SERVO,
    Pins::WRIST_SERVO
);

BluepadInput gamepad;
NetworkManager network;
UdpReceiver udp;
AutonomousController autonomous;

RobotController robot(
    motors,
    servos,
    gamepad,
    network,
    udp,
    autonomous
);

void setup() {
    Serial.begin(115200);
    delay(500);
    robot.begin();
}

void loop() {
    robot.update();
    delay(5);
}
