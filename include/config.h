#pragma once

#include <Arduino.h>

namespace MotorPins {
    constexpr uint8_t ENA = 14;

    constexpr uint8_t IN1 = 27;
    constexpr uint8_t IN2 = 12;

    constexpr uint8_t IN3 = 13;
    constexpr uint8_t IN4 = 15;

    constexpr uint8_t ENB = 25;
}

namespace ServoPins {
    constexpr uint8_t SERVO_1 = 19;
    constexpr uint8_t SERVO_2 = 18;
}

namespace PS4Config {
    // Ganti dengan MAC address ESP32
    // yang sudah dipair dengan controller.
    constexpr const char* MAC_ADDRESS =
        "1a:2b:3c:4d:5e:6f";

    constexpr int DEADZONE = 20;
}

namespace ServoConfig {
    // Sudut awal ketika ESP32 menyala
    constexpr int INITIAL_ANGLE = 90;

    // Sudut ketika grip terbuka
    constexpr int GRIP_OPEN_ANGLE = 60;

    // Sudut ketika grip tertutup
    constexpr int GRIP_CLOSE_ANGLE = 120;
}