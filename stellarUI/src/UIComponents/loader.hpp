#pragma once
#include "component.hpp"
#include "animation/animator.hpp"
#include <cmath>

class Loader : public Component {
private:
    int radius;
    uint16_t color;
    bool isVisible = false;
    uint32_t angle = 0;

    float rotationSpeed = 360.0f / 400.0f;
    uint32_t lastUpdateTime = 0;

    int centerX, centerY;

public:

    Loader(int r, uint16_t c) : radius(r), color(c) {}

    ~Loader() {}

    void setExist(bool exist) {
        isVisible = exist;
    }

    void setDisappear(bool disappear) {
        isVisible = !disappear;
    }

    void update(uint32_t now) override {
        if (isVisible) {
            uint32_t elapsedTime = now - lastUpdateTime;
            if (elapsedTime > 0) {
                angle += rotationSpeed * elapsedTime;
                if (angle >= 360.0f) {
                    angle -= 360.0f;
                }
            }
            lastUpdateTime = now;
        }
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!isVisible) return;
        buf.drawSmoothArc(buf.width()/2 + offsetX,buf.height()/2 + offsetY,radius,radius - 2, angle, (angle + 180) % 360, TFT_WHITE, buf.readPixel(buf.width()/2 + offsetX,buf.height()/2 + offsetY), true);
    }

    virtual bool containsPoint(int px, int py) override { return false; }
};
