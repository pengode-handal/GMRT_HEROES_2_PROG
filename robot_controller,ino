#include <Bluepad32.h>
#include <ESP32Servo.h>

// ==============================
// PIN MOTOR
// ==============================

#define MOTOR_LEFT_IN1   5
#define MOTOR_LEFT_IN2   4

#define MOTOR_RIGHT_IN1  2
#define MOTOR_RIGHT_IN2  15

// ==============================
// PIN SERVO
// ==============================

#define GRIPPER_SERVO_PIN 27

// ==============================
// KONFIGURASI MOTOR
// ==============================

#define MOTOR_MAX_PWM 255
#define MOTOR_DEADZONE 20

// Kalau salah satu motor berputar terbalik,
// ubah menjadi true.
bool LEFT_MOTOR_REVERSE = false;
bool RIGHT_MOTOR_REVERSE = false;

// ==============================
// KONFIGURASI GRIPPER
// ==============================

#define GRIPPER_OPEN_ANGLE  
#define GRIPPER_CLOSE_ANGLE  120
#define GRIPPER_INITIAL_ANGLE 90

// ==============================
// KONFIGURASI CONTROLLER
// ==============================

#define STICK_DEADZONE 30

// ==============================
// OBJECT
// ==============================

Servo gripper;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// ==============================
// MOTOR
// ==============================

void setupMotorPins() {
    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);

    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);

    stopMotors();
}

void setLeftMotor(int pwm) {
    if (LEFT_MOTOR_REVERSE) {
        pwm = -pwm;
    }

    pwm = constrain(pwm, -255, 255);

    if (pwm > 0) {
        analogWrite(MOTOR_LEFT_IN1, pwm);
        digitalWrite(MOTOR_LEFT_IN2, LOW);
    }
    else if (pwm < 0) {
        digitalWrite(MOTOR_LEFT_IN1, LOW);
        analogWrite(MOTOR_LEFT_IN2, abs(pwm));
    }
    else {
        digitalWrite(MOTOR_LEFT_IN1, LOW);
        digitalWrite(MOTOR_LEFT_IN2, LOW);
    }
}

void setRightMotor(int pwm) {
    if (RIGHT_MOTOR_REVERSE) {
        pwm = -pwm;
    }

    pwm = constrain(pwm, -255, 255);

    if (pwm > 0) {
        analogWrite(MOTOR_RIGHT_IN1, pwm);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
    }
    else if (pwm < 0) {
        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        analogWrite(MOTOR_RIGHT_IN2, abs(pwm));
    }
    else {
        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
    }
}

void setMotor(int leftPWM, int rightPWM) {
    leftPWM = constrain(leftPWM, -255, 255);
    rightPWM = constrain(rightPWM, -255, 255);

    setLeftMotor(leftPWM);
    setRightMotor(rightPWM);
}

void stopMotors() {
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, LOW);

    digitalWrite(MOTOR_RIGHT_IN1, LOW);
    digitalWrite(MOTOR_RIGHT_IN2, LOW);
}

// ==============================
// GRIPPER
// ==============================

void setupGripper() {
    gripper.attach(GRIPPER_SERVO_PIN);

    gripper.write(GRIPPER_INITIAL_ANGLE);

    Serial.println("Gripper initialized.");
}

void openGripper() {
    gripper.write(GRIPPER_OPEN_ANGLE);

    Serial.println("Gripper: OPEN");
}

void closeGripper() {
    gripper.write(GRIPPER_CLOSE_ANGLE);

    Serial.println("Gripper: CLOSE");
}

// ==============================
// DEADZONE
// ==============================

int applyDeadzone(int value) {
    if (abs(value) < STICK_DEADZONE) {
        return 0;
    }

    return value;
}

// ==============================
// CONTROLLER CONNECTED
// ==============================

void onConnectedController(ControllerPtr ctl) {

    bool foundEmptySlot = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {

        if (myControllers[i] == nullptr) {

            myControllers[i] = ctl;

            Serial.println();
            Serial.println("==============================");
            Serial.println("PS4 CONTROLLER CONNECTED");
            Serial.println("==============================");

            Serial.print("Controller index: ");
            Serial.println(i);

            Serial.print("Model: ");
            Serial.println(ctl->getModelName());

            foundEmptySlot = true;

            break;
        }
    }

    if (!foundEmptySlot) {
        Serial.println("No empty controller slot.");
    }
}

// ==============================
// CONTROLLER DISCONNECTED
// ==============================

void onDisconnectedController(ControllerPtr ctl) {

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {

        if (myControllers[i] == ctl) {

            myControllers[i] = nullptr;

            Serial.println();
            Serial.println("==============================");
            Serial.println("PS4 CONTROLLER DISCONNECTED");
            Serial.println("==============================");

            stopMotors();

            return;
        }
    }
}

// ==============================
// PROCESS CONTROLLER
// ==============================

void processGamepad(ControllerPtr gamepad) {

    if (!gamepad->isConnected()) {
        return;
    }

    if (!gamepad->hasData()) {
        return;
    }

    // ==========================
    // EMERGENCY STOP
    // ==========================

    // Cross pada PS4 = a() pada Bluepad32
    if (gamepad->a()) {
        stopMotors();

        Serial.println("EMERGENCY STOP");

        return;
    }

    // ==========================
    // LEFT STICK
    // ==========================

    int x = gamepad->axisX();
    int y = gamepad->axisY();

    x = applyDeadzone(x);
    y = applyDeadzone(y);

    // Bluepad32:
    // stick maju biasanya menghasilkan nilai negatif
    // sehingga dibalik agar maju = positif.

    y = -y;

    // ==========================
    // NORMALISASI
    // ==========================

    x = constrain(x, -511, 511);
    y = constrain(y, -511, 511);

    // ==========================
    // DIFFERENTIAL DRIVE
    // ==========================

    int leftPWM = y + x;
    int rightPWM = y - x;

    leftPWM = constrain(leftPWM, -511, 511);
    rightPWM = constrain(rightPWM, -511, 511);

    // Ubah range -511..511 menjadi -255..255

    leftPWM = map(leftPWM, -511, 511, -255, 255);
    rightPWM = map(rightPWM, -511, 511, -255, 255);

    // ==========================
    // MOTOR
    // ==========================

    setMotor(leftPWM, rightPWM);

    // ==========================
    // GRIPPER
    // ==========================

    if (gamepad->r1()) {
        openGripper();
    }

    if (gamepad->l1()) {
        closeGripper();
    }
}

// ==============================
// PROCESS ALL CONTROLLERS
// ==============================

void processControllers() {

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {

        ControllerPtr ctl = myControllers[i];

        if (ctl == nullptr) {
            continue;
        }

        if (!ctl->isConnected()) {
            continue;
        }

        if (!ctl->hasData()) {
            continue;
        }

        if (ctl->isGamepad()) {
            processGamepad(ctl);
        }
    }
}

// ==============================
// SETUP
// ==============================

void setup() {

    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("PS4 BLUEPAD32 ROBOT");
    Serial.println("==============================");

    setupMotorPins();

    setupGripper();

    // Bluepad32 setup
    BP32.setup(
        &onConnectedController,
        &onDisconnectedController
    );

    // Kita tidak menggunakan virtual controller
    BP32.enableVirtualDevice(false);

    Serial.println("Bluepad32 initialized.");
    Serial.println("Waiting for PS4 controller...");
}

// ==============================
// LOOP
// ==============================

void loop() {

    bool dataUpdated = BP32.update();

    if (dataUpdated) {
        processControllers();
    }

    delay(20);
}
