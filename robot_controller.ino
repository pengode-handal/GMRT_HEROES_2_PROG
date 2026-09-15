#include <Arduino.h>
#include <PS4Controller.h>
#include <ESP32Servo.h>

// =========================
// PIN MOTOR L298N
// =========================

#define MOTOR_IN1 5
#define MOTOR_IN2 4
#define MOTOR_IN3 2
#define MOTOR_IN4 15

// =========================
// PIN SERVO
// =========================

#define SERVO1_PIN 19
#define SERVO2_PIN 18

Servo servo1;
Servo servo2;

// Posisi awal servo
int servo1Angle = 90;
int servo2Angle = 90;

// =========================
// KONFIGURASI MOTOR
// =========================

// Kalau arah motor terbalik, ubah menjadi true
bool reverseLeftMotor = false;
bool reverseRightMotor = false;

// Deadzone joystick
const int DEADZONE = 20;

// Kecepatan perubahan servo
const int SERVO_STEP = 2;


// =========================
// SETUP
// =========================

void setup() {
  Serial.begin(115200);

  // Motor
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);

  // Pastikan motor mati saat startup
  stopMotor();

  // Servo
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  servo1.write(servo1Angle);
  servo2.write(servo2Angle);

  // PS4
  PS4.begin();

  Serial.println();
  Serial.println("==============================");
  Serial.println("PS4 ROBOT CONTROLLER");
  Serial.println("==============================");
  Serial.println("Waiting for PS4 controller...");
}


// =========================
// LOOP
// =========================

void loop() {

  if (PS4.isConnected()) {

    // Kontrol motor
    controlMotor();

    // Kontrol servo
    controlServo();

    // Tombol X untuk emergency stop
    if (PS4.Cross()) {
      stopMotor();
    }

    delay(20);
  }
  else {
    // Kalau controller terputus,
    // motor langsung dimatikan
    stopMotor();
  }
}


// =========================
// KONTROL MOTOR
// =========================

void controlMotor() {

  int x = PS4.LStickX();
  int y = PS4.LStickY();

  // Deadzone
  if (abs(x) < DEADZONE) {
    x = 0;
  }

  if (abs(y) < DEADZONE) {
    y = 0;
  }

  // Tidak ada input
  if (x == 0 && y == 0) {
    stopMotor();
    return;
  }

  // =========================
  // MAJU / MUNDUR
  // =========================

  if (y < -DEADZONE) {

    // Maju
    if (abs(x) < DEADZONE) {
      moveForward();
    }

    // Maju + belok kiri
    else if (x < -DEADZONE) {
      turnLeftForward();
    }

    // Maju + belok kanan
    else if (x > DEADZONE) {
      turnRightForward();
    }
  }

  // =========================
  // MUNDUR
  // =========================

  else if (y > DEADZONE) {

    // Mundur
    if (abs(x) < DEADZONE) {
      moveBackward();
    }

    // Mundur + belok kiri
    else if (x < -DEADZONE) {
      turnLeftBackward();
    }

    // Mundur + belok kanan
    else if (x > DEADZONE) {
      turnRightBackward();
    }
  }

  // =========================
  // BELAKAN / KANAN
  // =========================

  else {

    if (x < -DEADZONE) {
      turnLeft();
    }

    else if (x > DEADZONE) {
      turnRight();
    }

    else {
      stopMotor();
    }
  }
}


// =========================
// MOTOR MAJU
// =========================

void moveForward() {

  setLeftMotor(true);
  setRightMotor(true);
}


// =========================
// MOTOR MUNDUR
// =========================

void moveBackward() {

  setLeftMotor(false);
  setRightMotor(false);
}


// =========================
// BELOK KIRI
// =========================

void turnLeft() {

  setLeftMotor(false);
  setRightMotor(true);
}


// =========================
// BELOK KANAN
// =========================

void turnRight() {

  setLeftMotor(true);
  setRightMotor(false);
}


// =========================
// MAJU + KIRI
// =========================

void turnLeftForward() {

  // Motor kiri berhenti
  stopLeftMotor();

  // Motor kanan maju
  setRightMotor(true);
}


// =========================
// MAJU + KANAN
// =========================

void turnRightForward() {

  // Motor kiri maju
  setLeftMotor(true);

  // Motor kanan berhenti
  stopRightMotor();
}


// =========================
// MUNDUR + KIRI
// =========================

void turnLeftBackward() {

  // Motor kiri berhenti
  stopLeftMotor();

  // Motor kanan mundur
  setRightMotor(false);
}


// =========================
// MUNDUR + KANAN
// =========================

void turnRightBackward() {

  // Motor kiri mundur
  setLeftMotor(false);

  // Motor kanan berhenti
  stopRightMotor();
}


// =========================
// MOTOR KIRI
// =========================

void setLeftMotor(bool forward) {

  if (reverseLeftMotor) {
    forward = !forward;
  }

  if (forward) {
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
  }
  else {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
  }
}


// =========================
// MOTOR KANAN
// =========================

void setRightMotor(bool forward) {

  if (reverseRightMotor) {
    forward = !forward;
  }

  if (forward) {
    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);
  }
  else {
    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);
  }
}


// =========================
// STOP MOTOR KIRI
// =========================

void stopLeftMotor() {

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
}


// =========================
// STOP MOTOR KANAN
// =========================

void stopRightMotor() {

  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
}


// =========================
// STOP SEMUA MOTOR
// =========================

void stopMotor() {

  stopLeftMotor();
  stopRightMotor();
}


// =========================
// KONTROL SERVO
// =========================

void controlServo() {

  int rx = PS4.RStickX();
  int ry = PS4.RStickY();

  // Servo 1 menggunakan Right Stick X
  if (rx > 40) {
    servo1Angle += SERVO_STEP;
  }
  else if (rx < -40) {
    servo1Angle -= SERVO_STEP;
  }

  // Servo 2 menggunakan Right Stick Y
  if (ry > 40) {
    servo2Angle -= SERVO_STEP;
  }
  else if (ry < -40) {
    servo2Angle += SERVO_STEP;
  }

  // Batasi sudut servo
  servo1Angle = constrain(servo1Angle, 0, 180);
  servo2Angle = constrain(servo2Angle, 0, 180);

  servo1.write(servo1Angle);
  servo2.write(servo2Angle);
}