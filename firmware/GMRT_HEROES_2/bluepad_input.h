#pragma once

#include <Arduino.h>

class BluepadInput {
public:
    void begin();
    void update();

    bool connected() const;
    int leftX() const;
    int leftY() const;
    int rightX() const;
    int rightY() const;
    int gripThrottle() const;
    int brakeTrigger() const;

    bool crossPressed() const;
    bool circlePressed() const;
    bool trianglePressed() const;

private:
    bool _previousCross = false;
    bool _previousCircle = false;
    bool _previousTriangle = false;
    bool _crossPressed = false;
    bool _circlePressed = false;
    bool _trianglePressed = false;
};
