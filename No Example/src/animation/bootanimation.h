#pragma once

#include <TFT_eSPI.h>
#include <vector>
#include <cmath>
#include "config.h"

inline void BootAnimation(TFT_eSPI& tft, TFT_eSprite& buffer) {
    const int w = tft.width();
    const int h = tft.height();
    const int centerX = w / 2;
    const int centerY = h / 2;
    const int starCount = 80;
    const int frameInterval = 16;

    int duration = startupDuration;
    if (duration < 1600) duration = 1600;
    float gatherRatio = 0.4f;
    float pauseRatio = 0.05f;
    float explodeRatio = 0.2f;
    float textFadeRatio = 0.35f;
    int gatherDurationMs = duration * gatherRatio;
    int pauseDurationMs = duration * pauseRatio;
    int explodeDurationMs = duration * explodeRatio;
    int textFadeDurationMs = duration * textFadeRatio;

    const int gatherFrames = gatherDurationMs / frameInterval;
    const int pauseFrames = pauseDurationMs / frameInterval;
    const int explodeFrames = explodeDurationMs / frameInterval;
    const int textFadeFrames = textFadeDurationMs / frameInterval;
    const int totalFramesBeforeExplode = gatherFrames + pauseFrames;
    const float minGatherRadius = 0.0f;

    struct Star {
        float angle;
        float radius;
        float originalRadius;
        float angleSpeed;
        uint16_t color;
        float x, y;
        int targetX, targetY;
        float explodeDx, explodeDy;
    };

    std::vector<Star> stars;
    for (int i = 0; i < starCount; ++i) {
        float angle = random(0, 628) / 100.0f;
        float radius = random(30, 250);
        float angleSpeed = 0.1f + ((float)random(0, 100) / 2000.0f);
        uint16_t color = buffer.color565(random(200, 255), random(200, 255), random(200, 255));
        stars.push_back({angle, radius, radius, angleSpeed, color, 0, 0, centerX, centerY, 0, 0});
    }

    bool explodeInitialized = false;

    for (int frame = 0; frame < totalFramesBeforeExplode; ++frame) {
        buffer.fillRect(0, 0, w, h, TFT_BLACK);

        if (frame < gatherFrames) {
            float t = (float)frame / gatherFrames;
            float ease = 1 - pow(t, 2);
            for (auto& star : stars) {
                star.angle += star.angleSpeed * (1.0f - star.radius / 300.0f);
                float r = fmax(minGatherRadius, star.originalRadius * ease);
                float gx = centerX + cos(star.angle) * r;
                float gy = centerY + sin(star.angle) * r;
                star.x = gx * (1 - t) + star.targetX * t;
                star.y = gy * (1 - t) + star.targetY * t;
                buffer.fillSmoothCircle(star.x, star.y, 1, star.color);
            }
        } else {
            for (auto& star : stars) {
                star.x = star.targetX;
                star.y = star.targetY;
                star.color = buffer.color565(255, 255, 255);
                buffer.fillSmoothCircle(star.x, star.y, 1, star.color);
            }
        }

        buffer.pushSprite(0, 0);
        delay(frameInterval);
    }

    if (!explodeInitialized) {
        for (auto& star : stars) {
            float dirAngle = random(0, 628) / 100.0f;
            star.explodeDx = cos(dirAngle);
            star.explodeDy = sin(dirAngle);
            star.x = star.targetX;
            star.y = star.targetY;
            star.color = buffer.color565(255, 255, 255);
        }
        explodeInitialized = true;
    }

    bool allOffScreen = false;
    int explodeFrame = 0;

    while (!allOffScreen || explodeFrame < explodeFrames || explodeFrame < textFadeFrames) {
        buffer.fillRect(0, 0, w, h, TFT_BLACK);
        allOffScreen = true;

        float tExplode = (float)explodeFrame / explodeFrames;
        if (tExplode > 1.0f) tExplode = 1.0f;

        float tText = (float)explodeFrame / textFadeFrames;
        if (tText > 1.0f) tText = 1.0f;

        for (auto& star : stars) {
            float speed = 10.0f + (star.originalRadius / 250.0f) * 30.0f;
            star.x += star.explodeDx * speed;
            star.y += star.explodeDy * speed;

            if (star.x >= 0 && star.x < w && star.y >= 0 && star.y < h) {
                buffer.fillSmoothCircle(star.x, star.y, 1, star.color);
                allOffScreen = false;
            }
        }

        uint8_t c = (uint8_t)(tText * 255);
        uint16_t color = buffer.color565(c, c, c);
        buffer.setTextColor(color);
        buffer.setTextDatum(MC_DATUM);
        buffer.setFreeFont(&FreeSansBold18pt7b);
        buffer.drawString("StellarUI", centerX, centerY);
        buffer.pushSprite(0, 0);
        explodeFrame++;
        delay(frameInterval);
    }

    buffer.setFreeFont(NULL);
    delay(1000);
}
inline float easeInOutQuad(float t) {
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}
inline void BootAnimation2(TFT_eSPI& tft, TFT_eSprite& buffer) {
    const int w = tft.width();
    const int h = tft.height();
    const int centerX = w / 2;
    const int centerY = h / 2;
    const int starCount = 100;
    const int frameInterval = 16; // 约60FPS

    const int IWidth = 8;
    const int IHeight = 60;
    const int expandedWidth = 160;
    const int expandedHeight = 60;

    int duration = startupDuration;
    if (duration < 2000) duration = 2000;

    int gatherDuration = duration * 0.72f;
    int expandDuration = duration * 0.2f;
    int pauseDuration = duration * 0.08f;

    struct Star {
        float angle;
        float radius;
        float angleSpeed;
        uint16_t color;
        float x, y;
        float tx, ty;
    };

    std::vector<Star> stars;
    for (int i = 0; i < starCount; ++i) {
        float angle = random(0, 628) / 100.0f;
        float radius = random(40, 150);
        float angleSpeed = 0.05f + ((float)random(0, 100) / 3000.0f);
        uint16_t color = buffer.color565(random(180, 255), random(180, 255), random(180, 255));
        stars.push_back({angle, radius, angleSpeed, color, 0, 0, 0, 0});
    }

    for (auto& star : stars) {
        float ix = centerX - IWidth / 2 + random(0, IWidth);
        float iy = centerY - IHeight / 2 + random(0, IHeight);
        star.tx = ix;
        star.ty = iy;
    }

    unsigned long startTime = millis();
    unsigned long currentTime = startTime;

    while (true) {
        currentTime = millis();
        unsigned long elapsed = currentTime - startTime;
        if (elapsed > gatherDuration) break;

        float t = (float)elapsed / gatherDuration;
        float ease = 1 - pow(t, 2);

        buffer.fillRect(0, 0, w, h, TFT_BLACK);
        for (auto& star : stars) {
            star.angle += star.angleSpeed;
            float r = star.radius * ease;
            float gx = centerX + cos(star.angle) * r;
            float gy = centerY + sin(star.angle) * r;

            star.x = gx * (1 - t) + star.tx * t;
            star.y = gy * (1 - t) + star.ty * t;

            buffer.fillSmoothCircle(star.x, star.y, 1, star.color);
        }
        buffer.pushSprite(0, 0);
        delay(frameInterval);
    }

    buffer.fillRect(0, 0, w, h, TFT_BLACK);
    buffer.fillRect(centerX - IWidth / 2, centerY - IHeight / 2, IWidth, IHeight, TFT_WHITE);
    buffer.pushSprite(0, 0);
    delay(pauseDuration);

    buffer.setFreeFont(&FreeSansBold18pt7b);
    const char* text = "StellarUI";
    int textLen = strlen(text);

    startTime = millis();
    while (true) {
        currentTime = millis();
        unsigned long elapsed = currentTime - startTime;
        if (elapsed > expandDuration) break;

        float rawT = (float)elapsed / expandDuration;
        float tExpand = easeInOutQuad(rawT);

        buffer.fillSmoothRoundRect(0, 0, w, h, 4, TFT_BLACK);

        int currWidth = IWidth + (expandedWidth - IWidth) * tExpand;
        int currHeight = expandedHeight;
        int rectX = centerX - currWidth / 2;
        int rectY = centerY - currHeight / 2;

        buffer.fillSmoothRoundRect(rectX, rectY, currWidth, currHeight, (currWidth > 40) ? 20 : currWidth / 2, TFT_WHITE);

        int lettersToShow = (int)(textLen * tExpand);
        if (lettersToShow > textLen) lettersToShow = textLen;

        if (lettersToShow > 0) {
            String partialText = String(text).substring(0, lettersToShow);
            int textX = centerX;
            buffer.setTextColor(TFT_BLACK, TFT_WHITE);
            buffer.setTextDatum(MC_DATUM);
            buffer.drawString(partialText, textX, centerY);
        }

        buffer.pushSprite(0, 0);
        delay(frameInterval);
    }
    buffer.fillSmoothRoundRect(centerX - expandedWidth / 2, centerY - expandedHeight / 2, expandedWidth, expandedHeight, 20, TFT_WHITE);
    buffer.setTextColor(TFT_BLACK, TFT_WHITE);
    buffer.setTextDatum(MC_DATUM);
    buffer.drawString(text, centerX, centerY);
    buffer.pushSprite(0, 0);
    buffer.setFreeFont(NULL);
    delay(1000);
}
