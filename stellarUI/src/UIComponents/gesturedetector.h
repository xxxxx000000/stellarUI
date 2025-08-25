#pragma once
#include <functional>
#include "component.h"
#include "../services/screenrotationservice.h"

extern ScreenRotationService screenrotationservice;

class GestureDetector : public Component {
public:
    std::function<void()> onSwipeUp;
    std::function<void()> onSwipeDown;
    std::function<void()> onSwipeLeft;
    std::function<void()> onSwipeRight;

    int swipeThreshold;

    int width, height;
    float fw, fh;
    bool flexLayout = false;

    int screenWidth;
    int screenHeight;

    bool hasTriggered = false;

    GestureDetector(int _x, int _y, int _w, int _h, int _threshold = 30)
        : swipeThreshold(_threshold), width(_w), height(_h), flexLayout(false) {
        x = _x;
        y = _y;
        enabled = true;
        targetX = x;
        targetY = y;
        scale = targetScale = 1.0f;
    }

    GestureDetector(float _fx, float _fy, float _fw, float _fh, int _threshold = 30)
        : Component(_fx,_fy),swipeThreshold(_threshold), fw(_fw), fh(_fh), flexLayout(true) {
        enabled = true;
        scale = targetScale = 1.0f;
    }

    virtual bool containsPoint(int px, int py) override {
        if (!flexLayout) {
            int halfW = (int)(width * scale) / 2;
            int halfH = (int)(height * scale) / 2;
            return (px >= x - halfW && px <= x + halfW &&
                    py >= y - halfH && py <= y + halfH);
        } else {
            int cx = (int)(fx * screenWidth);
            int cy = (int)(fy * screenHeight);
            int halfW = (int)(fw * screenWidth * scale) / 2;
            int halfH = (int)(fh * screenHeight * scale) / 2;
            return (px >= cx - halfW && px <= cx + halfW &&
                    py >= cy - halfH && py <= cy + halfH);
        }
    }

    virtual void handleTouch(int tx, int ty) override {
        if (!enabled) return;

        if (tx >= 0 && ty >= 0) {
            if (!isTouching && containsPoint(tx, ty)) {
                isTouching = true;
                startX = tx;
                startY = ty;
                hasTriggered = false;
            } 
            else if (isTouching && !hasTriggered) {
                int dx = tx - startX;
                int dy = ty - startY;
                if (abs(dx) >= swipeThreshold || abs(dy) >= swipeThreshold) {
                    if (abs(dx) > abs(dy)) {
                        if (dx > 0 && onSwipeRight) onSwipeRight();
                        else if (dx < 0 && onSwipeLeft) onSwipeLeft();
                    } else {
                        if (dy > 0 && onSwipeDown) onSwipeDown();
                        else if (dy < 0 && onSwipeUp) onSwipeUp();
                    }
                    hasTriggered = true;
                }
            }
        } 
        else {
            isTouching = false;
            hasTriggered = false;
        }

        lastX = tx;
        lastY = ty;
    }

private:
    bool isTouching = false;
    int16_t startX = 0, startY = 0;
    int16_t lastX = -1, lastY = -1;

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        screenWidth = buf.width();
        screenHeight = buf.height();
    }
    virtual void update(uint32_t now) override {}
};