#include "bluepad_input.h"

#include <Bluepad32.h>

namespace {
ControllerPtr controller = nullptr;

void onControllerConnected(ControllerPtr connectedController) {
    if (controller != nullptr) {
        Serial.println("A controller is already active; ignoring the new controller.");
        return;
    }

    controller = connectedController;
    Serial.print("Bluepad32 controller connected: ");
    Serial.println(controller->getModelName());
}

void onControllerDisconnected(ControllerPtr disconnectedController) {
    if (controller == disconnectedController) {
        controller = nullptr;
        Serial.println("Bluepad32 controller disconnected.");
    }
}

bool controllerReady() {
    return controller != nullptr
        && controller->isConnected()
        && controller->isGamepad();
}
}

void BluepadInput::begin() {
    BP32.setup(&onControllerConnected, &onControllerDisconnected);
    Serial.println("Bluepad32 ready. Pair the PS4 controller with Share + PS.");
}

void BluepadInput::update() {
    BP32.update();

    bool cross = connected() && controller->a();
    bool circle = connected() && controller->b();
    bool triangle = connected() && controller->y();

    _crossPressed = cross && !_previousCross;
    _circlePressed = circle && !_previousCircle;
    _trianglePressed = triangle && !_previousTriangle;

    _previousCross = cross;
    _previousCircle = circle;
    _previousTriangle = triangle;
}

bool BluepadInput::connected() const {
    return controllerReady();
}

int BluepadInput::leftX() const {
    return connected() ? controller->axisX() : 0;
}

int BluepadInput::leftY() const {
    return connected() ? controller->axisY() : 0;
}

int BluepadInput::rightX() const {
    return connected() ? controller->axisRX() : 0;
}

int BluepadInput::rightY() const {
    return connected() ? controller->axisRY() : 0;
}

int BluepadInput::gripThrottle() const {
    return connected() ? controller->throttle() : 0;
}

int BluepadInput::brakeTrigger() const {
    return connected() ? controller->brake() : 0;
}

bool BluepadInput::crossPressed() const {
    return _crossPressed;
}

bool BluepadInput::circlePressed() const {
    return _circlePressed;
}

bool BluepadInput::trianglePressed() const {
    return _trianglePressed;
}
