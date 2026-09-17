#pragma once

#include "robot_types.h"

class AutonomousController {
public:
    MotorCommand calculate(float tx, float tz) const;
};

