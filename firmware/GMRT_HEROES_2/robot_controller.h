#pragma once

#include "autonomous_controller.h"
#include "bluepad_input.h"
#include "motor_driver.h"
#include "network_manager.h"
#include "robot_types.h"
#include "servo_controller.h"
#include "udp_receiver.h"

class RobotController {
public:
    RobotController(
        MotorDriver& motors,
        ServoController& servos,
        BluepadInput& gamepad,
        NetworkManager& network,
        UdpReceiver& udp,
        AutonomousController& autonomous
    );

    void begin();
    void update();

private:
    MotorDriver& _motors;
    ServoController& _servos;
    BluepadInput& _gamepad;
    NetworkManager& _network;
    UdpReceiver& _udp;
    AutonomousController& _autonomous;
    RobotMode _mode = RobotMode::MANUAL;
    bool _emergencyStop = false;
    uint32_t _lastStatusMs = 0;
    AutonomousState _autoState = AutonomousState::STOP_NO_POSE;

    void handleButtons(uint32_t nowMs);
    void setMode(RobotMode nextMode, uint32_t nowMs);
    void updateManual(uint32_t nowMs);
    void updateAutonomous(uint32_t nowMs);
    void updateNetwork(uint32_t nowMs);
    void printStatus(uint32_t nowMs);
    int applyDeadzone(int value) const;
};
