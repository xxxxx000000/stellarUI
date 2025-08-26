#pragma once
#include <Arduino.h>
#include "storageservice.hpp"

/*
    亮度服务，主要用于控制调节亮度
    亮度变化方式为平滑变换
    可以调节熄屏亮屏和正常平滑变化的速率
    与储存服务绑定
    Brightness service mainly used to control and adjust brightness
    Brightness changes smoothly
    Allows adjustment of the rates for screen off, screen on, and normal smooth transitions
    Bound to a storage service
*/

extern StorageService storageservice;

class BrightnessService {
private:
    int pin;
    float currentBrightness = 255.0f;
    float targetBrightness = 255.0f;
    float userBrightness = 255.0f;
    bool initialized = false;
    bool screenOn = true;

    uint32_t lastUpdate = 0;
    float changeSpeed = 5.0f;//调节亮度时的变化速率 Rate of change when adjusting brightness


    float screenOnSpeed = 8.0f;//亮屏时的变化速率 Rate of change when the screen is turned on
    float screenOffSpeed = 8.0f;//熄屏时的变化速率 Rate of change when the screen is turned off

public:
    BrightnessService(int _pin = 15) : pin(_pin) {}

    void begin() {
        pinMode(pin, OUTPUT);
        initialized = true;

        int stored = storageservice.getInt("brightness", 255);
        userBrightness = constrain(stored, 0, 255);
        targetBrightness = userBrightness;
        currentBrightness = targetBrightness;
        screenOn = true;
        applyBrightness();
    }

    void setBrightness(uint8_t level) {
        uint8_t newTarget = constrain(level, 0, 255);
        targetBrightness = newTarget;
        userBrightness = newTarget;

        int stored = storageservice.getInt("brightness", 255);
        if (stored != newTarget) {
            storageservice.putInt("brightness", newTarget);
        }
    }

    uint8_t getBrightness() const {
        return (uint8_t)targetBrightness;
    }

    void turnOn() {
        screenOn = true;
        targetBrightness = userBrightness;
    }

    void turnOff() {
        screenOn = false;
        targetBrightness = 0;
    }
    float getCurrentBrightness() const {
        return currentBrightness;
    }
    void update() {
        if (!initialized) return;

        uint32_t now = millis();
        if (now - lastUpdate < 16) return;
        lastUpdate = now;

        float step = screenOn ? screenOnSpeed : screenOffSpeed;

        if (abs(currentBrightness - targetBrightness) < 0.5f) {
            currentBrightness = targetBrightness;
            applyBrightness();
            return;
        }

        if (currentBrightness < targetBrightness) {
            currentBrightness = min(currentBrightness + step, targetBrightness);
        } else {
            currentBrightness = max(currentBrightness - step, targetBrightness);
        }

        applyBrightness();
    }

    void setScreenOnSpeed(float speed) {
        screenOnSpeed = speed;
    }

    void setScreenOffSpeed(float speed) {
        screenOffSpeed = speed;
    }

private:
    void applyBrightness() {
        analogWrite(pin, (uint8_t)currentBrightness);
    }
};
