#pragma once
#include <Arduino.h>

/*
    控制震动马达，用于提供震动反馈
    Control vibration motor for haptic feedback
*/

class MotorService {
private:
    uint32_t vibrationEndMillis = 0;
    int motorPin = -1;
    bool vibrating = false;

public:
    void begin(int pin) {
        motorPin = pin;
        pinMode(motorPin, OUTPUT);
        digitalWrite(motorPin, LOW);
    }

    void vibrate(uint32_t durationMs) {
        if (motorPin == -1) return;

        digitalWrite(motorPin, HIGH);
        vibrationEndMillis = millis() + durationMs;
        vibrating = true;
    }

    void update() {
        if (vibrating && millis() >= vibrationEndMillis) {
            digitalWrite(motorPin, LOW);
            vibrating = false;
        }
    }

    bool isVibrating() const {
        return vibrating;
    }
};
