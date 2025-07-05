#pragma once
#include <Arduino.h>
#include <FreeRTOS.h>

/*
    提供当前帧数
    Provides the current frame rate
*/

class FrameRateService {
private:
    TickType_t lastWakeTime = 0;
    const TickType_t frameIntervalTicks;

    uint32_t lastFpsMillis = 0;
    uint16_t frameCount = 0;
    uint16_t currentFps = 0;

public:
    explicit FrameRateService(uint32_t msPerFrame = 16) : frameIntervalTicks(pdMS_TO_TICKS(msPerFrame)) {}

    void begin() {
        lastWakeTime = xTaskGetTickCount();
        lastFpsMillis = millis();
        frameCount = 0;
        currentFps = 0;
    }

    void frameTick() {
        vTaskDelayUntil(&lastWakeTime, frameIntervalTicks);

        frameCount++;
        uint32_t now = millis();
        if (now - lastFpsMillis >= 1000) {
            currentFps = frameCount;
            frameCount = 0;
            lastFpsMillis = now;
        }
    }

    uint16_t getFPS() const {
        return currentFps;
    }
};
