#include "ps4_input.h"
#include "config.h"

void PS4Input::begin() {

    PS4.begin(PS4Config::MAC_ADDRESS);

    Serial.println("PS4 input initialized.");
}

bool PS4Input::connected() const {
    return PS4.isConnected();
}

int PS4Input::leftX() const {
    return PS4.LStickX();
}

int PS4Input::leftY() const {
    return PS4.LStickY();
}

bool PS4Input::cross() const {
    return PS4.Cross();
}

bool PS4Input::l1() const {
    return PS4.L1();
}

bool PS4Input::r1() const {
    return PS4.R1();
}