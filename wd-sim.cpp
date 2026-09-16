#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <cmath>


// WIFI & UDP CONFIG

const char* WIFI_SSID = "SM-M146B";
const char* WIFI_PASSWORD = "pahlevi2007";

constexpr uint16_t UDP_PORT = 1337;
constexpr uint32_t UDP_TIMEOUT_MS = 300;


// AUTONOMOUS CONFIG

constexpr int TARGET_MARKER_ID = 1;

// Distance stop.
constexpr float STOP_DISTANCE_M = 0.25f;

// Jika sudut lebih dari 20 derajat,
// robot hanya berputar tanpa maju.
constexpr float ALIGN_ONLY_ANGLE_DEG = 20.0f;

// 1 meter error menghasilkan 300 PWM.
constexpr float DISTANCE_KP = 300.0f;

// 1 derajat error menghasilkan 4 PWM.
constexpr float TURN_KP = 4.0f;

constexpr int MAX_FORWARD_PWM = 130;
constexpr int MAX_TURN_PWM = 110;

// DATA RESULT

struct MotorCommand {
    int leftPWM;
    int rightPWM;

    int forwardPWM;
    int turnPWM;

    float angleDeg;
    float distanceError;

    const char* state;
};


WiFiUDP udp;

char packetBuffer[128];

bool poseActive = false;
uint32_t lastPoseTime = 0;


// PWM CALCULATION

MotorCommand calculateMotorPWM(
    float tx,
    float tz
) {
    MotorCommand command = {};

    if (
        !isfinite(tx)
        || !isfinite(tz)
        || tz <= 0.0f
    ) {
        command.state = "STOP_INVALID_POSE";
        return command;
    }

    command.distanceError = (
        tz - STOP_DISTANCE_M
    );

    // Marker deket
    if (command.distanceError <= 0.0f) {
        command.state = "STOP_TARGET_REACHED";
        return command;
    }

    // Sudut marker terhadap kamera.
    float angleRad = atan2f(tx, tz);

    command.angleDeg = (
        angleRad * 180.0f / PI
    );

    // PWM maju proporsional terhadap jarak.
    command.forwardPWM = static_cast<int>(
        roundf(
            DISTANCE_KP
            * command.distanceError
        )
    );

    command.forwardPWM = constrain(
        command.forwardPWM,
        0,
        MAX_FORWARD_PWM
    );

    // PWM correction
    command.turnPWM = static_cast<int>(
        roundf(
            TURN_KP
            * command.angleDeg
        )
    );

    command.turnPWM = constrain(
        command.turnPWM,
        -MAX_TURN_PWM,
        MAX_TURN_PWM
    );

    // rotate only
    if (
        fabsf(command.angleDeg)
        > ALIGN_ONLY_ANGLE_DEG
    ) {
        command.forwardPWM = 0;

        if (command.angleDeg > 0) {
            command.state = "ALIGN_RIGHT";
        } else {
            command.state = "ALIGN_LEFT";
        }
    }
    // robot approach.
    else {
        if (command.angleDeg > 2.0f) {
            command.state = "APPROACH_RIGHT";
        }
        else if (command.angleDeg < -2.0f) {
            command.state = "APPROACH_LEFT";
        }
        else {
            command.state = "APPROACH_STRAIGHT";
        }
    }

    // Differential drive mixing.
    command.leftPWM = constrain(
        command.forwardPWM + command.turnPWM,
        -255,
        255
    );

    command.rightPWM = constrain(
        command.forwardPWM - command.turnPWM,
        -255,
        255
    );

    return command;
}

// OUTPUT

void printMotorCommand(
    unsigned long sequence,
    int markerId,
    float tx,
    float tz,
    const MotorCommand& command
) {
    Serial.println();
    Serial.println("========== SIMULASI MOTOR ==========");

    Serial.printf(
        "Sequence       : %lu\n",
        sequence
    );

    Serial.printf(
        "Marker ID      : %d\n",
        markerId
    );

    Serial.printf(
        "tx             : %.3f m\n",
        tx
    );

    Serial.printf(
        "tz             : %.3f m\n",
        tz
    );

    Serial.printf(
        "Sudut          : %.2f deg\n",
        command.angleDeg
    );

    Serial.printf(
        "Error jarak    : %.3f m\n",
        command.distanceError
    );

    Serial.printf(
        "PWM maju       : %d\n",
        command.forwardPWM
    );

    Serial.printf(
        "PWM belok      : %d\n",
        command.turnPWM
    );

    Serial.printf(
        "PWM motor kiri : %d\n",
        command.leftPWM
    );

    Serial.printf(
        "PWM motor kanan: %d\n",
        command.rightPWM
    );

    Serial.printf(
        "State          : %s\n",
        command.state
    );

    Serial.println("====================================");
}


void printStop(const char* reason) {
    Serial.println();
    Serial.println("========== SIMULASI MOTOR ==========");
    Serial.println("PWM motor kiri : 0");
    Serial.println("PWM motor kanan: 0");

    Serial.print("State          : ");
    Serial.println(reason);

    Serial.println("====================================");
}
// WIFI

void connectWiFi() {
    WiFi.setHostname("gmrt-pwm-simulator");
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);

    Serial.print("Menghubungkan ke WiFi: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    uint32_t startTime = millis();

    while (
        WiFi.status() != WL_CONNECTED
        && millis() - startTime < 15000
    ) {
        Serial.print(".");
        delay(500);
    }

    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi gagal terhubung.");
        return;
    }

    Serial.println("WiFi terhubung.");

    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    if (udp.begin(UDP_PORT)) {
        Serial.print("UDP aktif pada port: ");
        Serial.println(UDP_PORT);
    } else {
        Serial.println("UDP gagal dimulai.");
    }
}


// PARSING UDP

void processUdpPacket() {
    int packetSize = udp.parsePacket();

    if (packetSize <= 0) {
        return;
    }

    int length = udp.read(
        packetBuffer,
        sizeof(packetBuffer) - 1
    );

    if (length <= 0) {
        return;
    }

    packetBuffer[length] = '\0';

    Serial.print("UDP diterima: ");
    Serial.println(packetBuffer);

    // Paket pose:
    // P,sequence,id,tx,tz
    if (packetBuffer[0] == 'P') {
        unsigned long sequence;
        int markerId;
        float tx;
        float tz;

        int parsed = sscanf(
            packetBuffer,
            "P,%lu,%d,%f,%f",
            &sequence,
            &markerId,
            &tx,
            &tz
        );

        if (parsed != 4) {
            poseActive = false;
            printStop("STOP_INVALID_PACKET");
            return;
        }

        if (markerId != TARGET_MARKER_ID) {
            poseActive = false;
            printStop("STOP_WRONG_MARKER");
            return;
        }

        poseActive = true;
        lastPoseTime = millis();

        MotorCommand command = calculateMotorPWM(
            tx,
            tz
        );

        printMotorCommand(
            sequence,
            markerId,
            tx,
            tz,
            command
        );
    }

    // Paket tidak ada marker:
    // N,sequence
    else if (packetBuffer[0] == 'N') {
        poseActive = false;
        printStop("STOP_NO_MARKER");
    }

    else {
        poseActive = false;
        printStop("STOP_UNKNOWN_PACKET");
    }
}


void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("UDP TO PWM SIMULATOR");
    Serial.println("Tidak ada motor yang digerakkan.");

    connectWiFi();
}


void loop() {
    processUdpPacket();
    if (
        poseActive
        && millis() - lastPoseTime
            > UDP_TIMEOUT_MS
    ) {
        poseActive = false;
        printStop("STOP_UDP_TIMEOUT");
    }

    delay(5);
}
