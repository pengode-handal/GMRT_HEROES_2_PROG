#pragma once

#include <Arduino.h>

#if __has_include("secrets.h")
#include "secrets.h"
#endif

#ifndef ROBOT_WIFI_SSID
#define ROBOT_WIFI_SSID "SM-M146B"
#endif

#ifndef ROBOT_WIFI_PASSWORD
#define ROBOT_WIFI_PASSWORD "pahlevi2007"
#endif

#ifndef ROBOT_SIMULATION
#define ROBOT_SIMULATION 0
#endif

namespace Pins {
constexpr uint8_t MOTOR_LEFT_IN1 = 5;
constexpr uint8_t MOTOR_LEFT_IN2 = 4;
constexpr uint8_t MOTOR_RIGHT_IN1 = 2;
constexpr uint8_t MOTOR_RIGHT_IN2 = 15;

constexpr uint8_t GRIP_SERVO = 27;
constexpr uint8_t ARM_SERVO = 19;
constexpr uint8_t WRIST_SERVO = 18;

constexpr uint8_t BUTTON_PIN = 25;
}

namespace MotorConfig {
constexpr bool LEFT_REVERSED = false;
constexpr bool RIGHT_REVERSED = false;
constexpr int PWM_LIMIT = 255;
constexpr int MANUAL_PWM_LIMIT = 200;
}

namespace ServoConfig {
constexpr int PULSE_MIN_US = 500;
constexpr int PULSE_MAX_US = 2400;

// MG90S starts fully open. Adjust these two limits to match the linkage.
constexpr int GRIP_OPEN_DEG = 0;
constexpr int GRIP_CLOSED_DEG = 120;
constexpr int GRIP_INITIAL_DEG = GRIP_OPEN_DEG;
constexpr int GRIP_TRIGGER_MAX = 1023;

constexpr int ARM_INITIAL_DEG = 90;
constexpr int ARM_MIN_DEG = 25;
constexpr int ARM_MAX_DEG = 155;

constexpr int WRIST_INITIAL_DEG = 90;
constexpr int WRIST_MIN_DEG = 20;
constexpr int WRIST_MAX_DEG = 160;

constexpr int STICK_DEADZONE = 100;
constexpr int STEP_DEG = 2;
constexpr uint32_t UPDATE_INTERVAL_MS = 20;
}

namespace GamepadConfig {
constexpr int AXIS_LIMIT = 512;
constexpr int DRIVE_DEADZONE = 55;
constexpr int THROTTLE_SIGN = -1;
constexpr int STEERING_SIGN = 1;
constexpr int TRIGGER_MAX = 1023;
constexpr int FULL_BRAKE_THRESHOLD = 1000;
}

namespace NetworkConfig {
constexpr const char* SSID = ROBOT_WIFI_SSID;
constexpr const char* PASSWORD = ROBOT_WIFI_PASSWORD;
constexpr const char* HOSTNAME = "gmrt-heroes-2";
constexpr const char* MDNS_SERVICE = "gmrt-udp";
constexpr const char* MDNS_PROTOCOL = "udp";
constexpr uint32_t RETRY_INTERVAL_MS = 10000;
constexpr uint32_t MDNS_RETRY_INTERVAL_MS = 5000;
}

namespace UdpConfig {
constexpr uint16_t PORT = 4210;
constexpr size_t BUFFER_SIZE = 128;
constexpr uint32_t POSE_TIMEOUT_MS = 300;
constexpr int TARGET_MARKER_ID = 1;
}

namespace AutonomousConfig {
constexpr float STOP_DISTANCE_M = 0.05f;
constexpr float ALIGN_ONLY_ANGLE_DEG = 20.0f;
constexpr float STRAIGHT_DEADBAND_DEG = 2.0f;
constexpr float DISTANCE_KP = 300.0f;
constexpr float TURN_KP = 4.0f;
constexpr int MAX_FORWARD_PWM = 130;
constexpr int MAX_TURN_PWM = 110;
constexpr int CAMERA_X_SIGN = 1;
}

namespace SystemConfig {
constexpr bool START_IN_AUTONOMOUS = false;
constexpr uint32_t STATUS_INTERVAL_MS = 500;
constexpr bool SIMULATION = ROBOT_SIMULATION != 0;
}
