#include <Bluepad32.h>

// 68:09:47:86:89:6A
// 14:2B:2F:DB:1A:F2

ControllerPtr controller = nullptr;

void onConnectedController(ControllerPtr connectedController) {
    if (controller == nullptr) {
        controller = connectedController;
        Serial.print("Connected: ");
        Serial.println(controller->getModelName());
    }
}

void onDisconnectedController(ControllerPtr disconnectedController) {
    if (controller == disconnectedController) {
        controller = nullptr;
        Serial.println("Controller disconnected.");
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    Serial.println("Hold Share + PS until the light blinks rapidly.");
}

void loop() {
    BP32.update();

    if (controller != nullptr && controller->isConnected() && controller->isGamepad()) {
        Serial.printf(
            "LX=%ld LY=%ld RX=%ld RY=%ld L2(brake)=%ld R2(grip)=%ld "
            "cross=%d circle=%d triangle=%d\n",
            static_cast<long>(controller->axisX()),
            static_cast<long>(controller->axisY()),
            static_cast<long>(controller->axisRX()),
            static_cast<long>(controller->axisRY()),
            static_cast<long>(controller->brake()),
            static_cast<long>(controller->throttle()),
            controller->a(),
            controller->b(),
            controller->y()
        );
    }

    delay(100);
}
