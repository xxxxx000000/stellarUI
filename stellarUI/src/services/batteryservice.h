#pragma once
#include <Arduino.h>

/*
    电池服务，主要用于提供电量。可根据自己的开发板修改，优化或者重写
    Battery service used to provide battery level information.
    Can be modified, optimized, or rewritten according to your own development board.

*/

class BatteryService {
private:
    int analogPin;
    float vRef;
    float r1, r2;

    float voltage = 0.0f;
    float percentage = 100.0f;
    bool charging = false;

    float lastVoltage = 0.0f;
    uint32_t lastUpdate = 0;
    uint16_t updateInterval = 1000;

    int chargeCounter = 0;
    const int chargeThreshold = 3;
    const int dischargeThreshold = 3;

    static const int filterSize = 8;
    float voltageBuffer[filterSize] = {0};
    int bufferIndex = 0;
    bool bufferFilled = false;

public:
    BatteryService(int _pin, float _vRef = 3.3, float _r1 = 200000.0, float _r2 = 100000.0)
        : analogPin(_pin), vRef(_vRef), r1(_r1), r2(_r2) {}

    void begin() {
        pinMode(analogPin, INPUT);
        lastUpdate = millis();
        for (int i = 0; i < filterSize; i++) {
            voltageBuffer[i] = 0.0f;
        }
        bufferIndex = 0;
        bufferFilled = false;
        lastVoltage = 0.0f;
        chargeCounter = 0;
        charging = false;
    }

    void update() {
        uint32_t now = millis();
        if (now - lastUpdate < updateInterval) return;
        lastUpdate = now;

        int raw = analogRead(analogPin);
        float vOut = (raw / 4095.0f) * vRef;
        float newVoltage = vOut * ((r1 + r2) / r2);

        voltageBuffer[bufferIndex] = newVoltage;
        bufferIndex = (bufferIndex + 1) % filterSize;
        if (bufferIndex == 0) bufferFilled = true;

        int count = bufferFilled ? filterSize : bufferIndex;
        float sum = 0;
        for (int i = 0; i < count; ++i) sum += voltageBuffer[i];
        float filteredVoltage = sum / count;

        voltage = filteredVoltage;
        percentage = voltageToPercentage(filteredVoltage);

        if (filteredVoltage > lastVoltage + 0.005f) {
            if (chargeCounter < chargeThreshold) chargeCounter++;
        } else if (filteredVoltage < lastVoltage - 0.005f) {
            if (chargeCounter > -dischargeThreshold) chargeCounter--;
        }
        lastVoltage = filteredVoltage;

        if (chargeCounter >= chargeThreshold) {
            charging = true;
        } else if (chargeCounter <= -dischargeThreshold) {
            charging = false;
        }
    }

    float getVoltage() const {
        return voltage;
    }

    float getPercentage() const {
        return percentage;
    }

    bool isCharging() const {
        return charging;
    }

private:
    float voltageToPercentage(float v) {
        const float maxV = 4.2;
        const float minV = 3.0;

        if (v >= maxV) return 100.0f;
        if (v <= minV) return 0.0f;
        return ((v - minV) / (maxV - minV)) * 100.0f;
    }
};
