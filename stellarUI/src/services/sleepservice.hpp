#pragma once
#include <Arduino.h>
#include "esp_sleep.h"
#include "brightnessservice.hpp"
#include "../config.hpp"

/*
    与亮度服务绑定，用于让ESP32进行轻度睡眠，按需修改或优化
    Bound to the brightness service, used to enable light sleep mode on the ESP32
    Modify or optimize as needed
*/

extern BrightnessService brightnessservice;

class SleepService {
private:
    uint32_t lastTouchMillis = 0;
    uint32_t timeout = AUTO_LOCK_TIME * 1000;
    gpio_num_t wakeupPin = GPIO_NUM_14;
    bool sleeping = false;

public:
    void setPreventSleep(bool value) {
        preventSleep = value;
    }

    bool isPreventingSleep() const {
        return preventSleep;
    }
    void begin(uint32_t timeoutMs = AUTO_LOCK_TIME * 1000, gpio_num_t wakePin = GPIO_NUM_14) {
        timeout = timeoutMs;
        wakeupPin = wakePin;
        lastTouchMillis = millis();

        esp_sleep_enable_ext0_wakeup(wakeupPin, 0);
    }

    void touchEvent() {
        lastTouchMillis = millis();

        if (sleeping) {
            brightnessservice.turnOn();
            waitForBrightnessTarget(brightnessservice.getCurrentBrightness());
            sleeping = false;
        }
    }

    void update() {
        if (sleeping) return;
        if (preventSleep) return;
        if (millis() - lastTouchMillis >= timeout) {
            goToSleep();
        }
    }

private:
    bool preventSleep = false;
    void goToSleep() {
        sleeping = true;

        brightnessservice.turnOff();
        waitForBrightnessTarget(0);

        esp_light_sleep_start();
    }

    void waitForBrightnessTarget(float target) {
        uint32_t start = millis();
        while (abs(brightnessservice.getCurrentBrightness() - target) > 1.0f) {
            brightnessservice.update();
            delay(16);
            if (millis() - start > 2000) break;
        }
    }
};
