#pragma once
#include <vector>
#include "component.h"
#include <cstdlib>
#include <ctime>
#include <Arduino.h>

class StarryBackground : public Component {
private:
    struct Star {
        float x, y;
        float dx, dy;
        uint8_t brightness;
        bool isMeteor = false;
        int tailLength = 0;

        Star(float _x, float _y, float _dx, float _dy, uint8_t _brightness, bool _isMeteor, int _tailLength)
            : x(_x), y(_y), dx(_dx), dy(_dy), brightness(_brightness), isMeteor(_isMeteor), tailLength(_tailLength) {}

        Star() : x(0), y(0), dx(0), dy(0), brightness(255), isMeteor(false), tailLength(0) {}
    };

    std::vector<Star> stars;
    int screenW, screenH;
    int starCount;
    int maxStars;
    int maxMeteors = 5;

    uint32_t lastMeteorTime = 0;
    uint32_t lastUpdateTime = 0;

public:
    StarryBackground(int _screenW, int _screenH, int count = 50)
        : screenW(_screenW), screenH(_screenH), starCount(count), maxStars(count + maxMeteors) {
        x = 0;
        y = 0;
        enabled = true;
        scale = targetScale = 1.0f;

        randomSeed(micros());
        for (int i = 0; i < starCount; ++i) {
            stars.push_back(generateStar());
        }
    }

    void update(uint32_t now) override {
        if (lastUpdateTime == 0) lastUpdateTime = now;
        float deltaTime = (now - lastUpdateTime) / 1000.0f;
        lastUpdateTime = now;

        for (auto& star : stars) {
            static constexpr float brightnessSpeed = 100.0f;
            float deltaBrightness = random(-10, 11) * brightnessSpeed * deltaTime;
            star.brightness = constrain(star.brightness + deltaBrightness, 80, 255);

            star.x += star.dx * deltaTime * 60;
            star.y += star.dy * deltaTime * 60;

            if (star.x < 0 || star.x >= screenW || star.y < 0 || star.y >= screenH) {
                star = generateStar();
            }
        }

        if (now - lastMeteorTime > 5000 && random(100) < 5) {
            lastMeteorTime = now;
            addMeteor();
        }
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        for (const auto& star : stars) {
            uint16_t color = buf.color565(star.brightness, star.brightness, star.brightness);
            int sx = static_cast<int>(star.x) + offsetX;
            int sy = static_cast<int>(star.y) + offsetY;

            if (star.isMeteor) {
                for (int i = 0; i < star.tailLength; ++i) {
                    int tx = sx - i * star.dx;
                    int ty = sy - i * star.dy;
                    if (tx >= 0 && tx < screenW && ty >= 0 && ty < screenH) {
                        uint8_t tailB = star.brightness * (1.0f - i / float(star.tailLength));
                        uint16_t tailColor = buf.color565(tailB, tailB, tailB);
                        buf.drawPixel(tx, ty, tailColor);
                    }
                }
            } else {
                if (sx >= 0 && sx < screenW && sy >= 0 && sy < screenH)
                    buf.drawPixel(sx, sy, color);
            }
        }
    }

    bool containsPoint(int px, int py) override {
        return false;
    }

private:
    Star generateStar() {
        return Star(
            float(random(screenW)),
            float(random(screenH)),
            random(-10, 11) / 30.0f,
            random(-10, 11) / 30.0f,
            static_cast<uint8_t>(random(100, 255)),
            false,
            0
        );
    }

    void addMeteor() {
        int currentMeteors = 0;
        for (auto& s : stars) {
            if (s.isMeteor) currentMeteors++;
        }
        if (currentMeteors >= maxMeteors) return;

        for (auto& s : stars) {
            if (!s.isMeteor) {
                s.x = random(0, screenW / 2);
                s.y = random(0, screenH / 2);
                s.dx = 1.5f;
                s.dy = 1.0f;
                s.brightness = 255;
                s.isMeteor = true;
                s.tailLength = 10;
                return;
            }
        }

        if ((int)stars.size() < maxStars) {
            Star meteor(
                random(0, screenW / 2),
                random(0, screenH / 2),
                1.5f,
                1.0f,
                255,
                true,
                10
            );
            stars.push_back(meteor);
        }
    }
};
