#pragma once

#include <Arduino.h>
#include <PS4Controller.h>

class PS4Input {
public:
    void begin();

    bool connected() const;

    int leftX() const;
    int leftY() const;

    bool cross() const;

    bool l1() const;
    bool r1() const;
};