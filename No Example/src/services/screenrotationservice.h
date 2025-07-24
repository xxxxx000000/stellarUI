#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern TFT_eSprite buffer;

class ScreenRotationService {
private:
    uint8_t rotation = tft.getRotation();
    int lastWidth = 0;
    int lastHeight = 0;

public:
    uint8_t getRotation() {
        return tft.getRotation();
    }

    void setRotation(uint8_t newRotation) {
        buffer.fillScreen(TFT_BLACK);
        buffer.deleteSprite();
        delay(100);
        tft.setRotation(newRotation);
        buffer.createSprite(tft.width(),tft.height());
    }
};
