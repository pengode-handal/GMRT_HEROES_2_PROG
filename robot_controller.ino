#include <Bluepad32.h>
#include <ESP32Servo.h>

// ==============================
// PIN MOTOR
// ==============================

// Motor kiri
#define MOTOR_LEFT_IN1   5
#define MOTOR_LEFT_IN2   4

// Motor kanan
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

// Kecepatan perubahan PWM
// Semakin besar = semakin responsif
#define PWM_ACCEL_STEP 18

// Perlambatan saat joystick dilepas
// Semakin besar = semakin cepat berhenti
#define PWM_BRAKE_STEP 30

// ==============================
// ARAH MOTOR
// ==============================

// Ubah menjadi true jika arah motor terbalik
bool LEFT_MOTOR_REVERSE = false;
bool RIGHT_MOTOR_REVERSE = false;

// ==============================
// KONFIGURASI GRIPPER
// ==============================

const int GRIPPER_OPEN_ANGLE = 30;
const int GRIPPER_CLOSE_ANGLE = 120;
const int GRIPPER_INITIAL_ANGLE = 90;

// ==============================
// KONFIGURASI JOYSTICK
// ==============================

#define STICK_DEADZONE 35

// ==============================
// OBJECT
// ==============================

Servo gripper;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// PWM motor yang sedang diberikan
int currentLeftPWM = 0;
int currentRightPWM = 0;

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

// ==============================
// MOTOR KIRI
// ==============================

void setLeftMotor(int pwm) {

    if (LEFT_MOTOR_REVERSE) {
        pwm = -pwm;
    }

    pwm = constrain(pwm, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);

    if (pwm > 0) {

        analogWrite(MOTOR_LEFT_IN1, pwm);
        digitalWrite(MOTOR_LEFT_IN2, LOW);

    } else if (pwm < 0) {

        digitalWrite(MOTOR_LEFT_IN1, LOW);
        analogWrite(MOTOR_LEFT_IN2, -pwm);

    } else {

        digitalWrite(MOTOR_LEFT_IN1, LOW);
        digitalWrite(MOTOR_LEFT_IN2, LOW);
    }
}

// ==============================
// MOTOR KANAN
// ==============================

void setRightMotor(int pwm) {

    if (RIGHT_MOTOR_REVERSE) {
        pwm = -pwm;
    }

    pwm = constrain(pwm, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);

    if (pwm > 0) {

        analogWrite(MOTOR_RIGHT_IN1, pwm);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);

    } else if (pwm < 0) {

        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        analogWrite(MOTOR_RIGHT_IN2, -pwm);

    } else {

        digitalWrite(MOTOR_RIGHT_IN1, LOW);
        digitalWrite(MOTOR_RIGHT_IN2, LOW);
    }
}

// ==============================
// SET MOTOR
// ==============================

void setMotor(int leftPWM, int rightPWM) {

    leftPWM = constrain(leftPWM, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);
    rightPWM = constrain(rightPWM, -MOTOR_MAX_PWM, MOTOR_MAX_PWM);

    setLeftMotor(leftPWM);
    setRightMotor(rightPWM);
}

// ==============================
// STOP MOTOR
// ==============================

void stopMotors() {

    currentLeftPWM = 0;
    currentRightPWM = 0;

    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, LOW);

    digitalWrite(MOTOR_RIGHT_IN1, LOW);
    digitalWrite(MOTOR_RIGHT_IN2, LOW);
}

// ==============================
// SMOOTH PWM
// ==============================

int movePWM(int currentPWM, int targetPWM) {

    // Kalau target lebih besar dari current,
    // PWM dinaikkan secara bertahap.

    if (targetPWM > currentPWM) {

        currentPWM += PWM_ACCEL_STEP;

        if (currentPWM > targetPWM) {
            currentPWM = targetPWM;
        }
    }

    // Kalau target lebih kecil dari current,
    // PWM diturunkan secara bertahap.

    else if (targetPWM < currentPWM) {

        currentPWM -= PWM_BRAKE_STEP;

        if (currentPWM < targetPWM) {
            currentPWM = targetPWM;
        }
    }

    return currentPWM;
}

// ==============================
// UPDATE MOTOR
// ==============================

void updateMotorPWM(int targetLeftPWM, int targetRightPWM) {

    targetLeftPWM = constrain(
        targetLeftPWM,
        -MOTOR_MAX_PWM,
        MOTOR_MAX_PWM
    );

    targetRightPWM = constrain(
        targetRightPWM,
        -MOTOR_MAX_PWM,
        MOTOR_MAX_PWM
    );

    // ==========================
    // CEGAH LANGSUNG BALIK ARAH
    // ==========================

    // Motor kiri
    if (
        currentLeftPWM != 0 &&
        targetLeftPWM != 0 &&
        ((currentLeftPWM > 0 && targetLeftPWM < 0) ||
         (currentLeftPWM < 0 && targetLeftPWM > 0))
    ) {

        targetLeftPWM = 0;
    }

    // Motor kanan
    if (
        currentRightPWM != 0 &&
        targetRightPWM != 0 &&
        ((currentRightPWM > 0 && targetRightPWM < 0) ||
         (currentRightPWM < 0 && targetRightPWM > 0))
    ) {

        targetRightPWM = 0;
    }

    // ==========================
    // RAMP PWM
    // ==========================

    currentLeftPWM = movePWM(
        currentLeftPWM,
        targetLeftPWM
    );

    currentRightPWM = movePWM(
        currentRightPWM,
        targetRightPWM
    );

    // ==========================
    // KIRIM PWM
    // ==========================

    setMotor(
        currentLeftPWM,
        currentRightPWM
    );
}

// ==============================
// GRIPPER
// ==============================

void setupGripper() {

    gripper.setPeriodHertz(50);

    gripper.attach(
        GRIPPER_SERVO_PIN,
        500,
        2400
    );

    gripper.write(GRIPPER_INITIAL_ANGLE);

    Serial.println("Gripper initialized.");
}

// ==============================
// BUKA GRIPPER
// ==============================

void openGripper() {

    gripper.write(GRIPPER_OPEN_ANGLE);
}

// ==============================
// TUTUP GRIPPER
// ==============================

void closeGripper() {

    gripper.write(GRIPPER_CLOSE_ANGLE);
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

            return;
        }
    }

    Serial.println("No empty controller slot.");
}

// ==============================
// CONTROLLER DISCONNECTED
// ==============================

void onDisconnectedController(ControllerPtr ctl) {

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {

        if (myControllers[i] == ctl) {

            myControllers[i] = nullptr;

            stopMotors();

            Serial.println();
            Serial.println("==============================");
            Serial.println("PS4 CONTROLLER DISCONNECTED");
            Serial.println("==============================");

            return;
        }
    }
}

// ==============================
// PROCESS GAMEPAD
// ==============================

void processGamepad(ControllerPtr gamepad) {

    if (!gamepad->isConnected()) {
        return;
    }

    // ==========================
    // EMERGENCY STOP
    // ==========================

    if (gamepad->a()) {

        stopMotors();

        return;
    }

    // ==========================
    // LEFT JOYSTICK
    // ==========================

    int x = gamepad->axisX();
    int y = gamepad->axisY();

    // Deadzone
    x = applyDeadzone(x);
    y = applyDeadzone(y);

    // Joystick atas = maju
    y = -y;

    // ==========================
    // BATASI NILAI
    // ==========================

    x = constrain(x, -511, 511);
    y = constrain(y, -511, 511);

    // ==========================
    // DIFFERENTIAL DRIVE
    // ==========================

    int leftValue = y + x;
    int rightValue = y - x;

    leftValue = constrain(
        leftValue,
        -511,
        511
    );

    rightValue = constrain(
        rightValue,
        -511,
        511
    );

    // ==========================
    // KONVERSI KE PWM
    // ==========================

    int leftPWM = map(
        leftValue,
        -511,
        511,
        -MOTOR_MAX_PWM,
        MOTOR_MAX_PWM
    );

    int rightPWM = map(
        rightValue,
        -511,
        511,
        -MOTOR_MAX_PWM,
        MOTOR_MAX_PWM
    );

    // ==========================
    // UPDATE MOTOR
    // ==========================

    updateMotorPWM(
        leftPWM,
        rightPWM
    );

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
// PROCESS CONTROLLERS
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

        if (!ctl->isGamepad()) {
            continue;
        }

        processGamepad(ctl);
    }
}

// ==============================
// SETUP
// ==============================

void setup() {

    Serial.begin(115200);

    delay(500);

    Serial.println();
    Serial.println("==============================");
    Serial.println("PS4 BLUEPAD32 ROBOT");
    Serial.println("==============================");

    setupMotorPins();

    setupGripper();

    BP32.setup(
        &onConnectedController,
        &onDisconnectedController
    );

    BP32.enableVirtualDevice(false);

    Serial.println("Bluepad32 initialized.");
    Serial.println("Waiting for PS4 controller...");
}

// ==============================
// LOOP
// ==============================

void loop() {

    BP32.update();

    processControllers();

    delay(5);
}
