#pragma once
#include <TFT_eSPI.h>
#include "component.h"
#include <functional>

/*
    图标组件，允许通过回调绘制自定义图标
    Icon component that uses a draw callback for custom rendering
*/

class IconComponent : public Component {
private:
    int size = 32;
    std::function<void(TFT_eSprite&, int centerX, int centerY, float scale)> drawCallback;

public:
    IconComponent(int _x, int _y,
                  std::function<void(TFT_eSprite&, int, int, float)> _drawCallback,
                  uint16_t _color = TFT_WHITE)
        : Component(_x, _y, _color), drawCallback(_drawCallback) {
        targetX = x;
        targetY = y;
        targetScale = scale = 1.0f;
        enabled = true;
    }

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled || !drawCallback) return;

        int drawX = x + offsetX;
        int drawY = y + offsetY;

        drawCallback(buf, drawX, drawY, scale);
    }

    virtual void update(uint32_t now) override {
        Component::update(now);
    }

    virtual bool containsPoint(int px, int py) override {
        if (!enabled) return false;

        int half = (int)(size * scale) / 2;
        return (px >= x - half) && (px <= x + half) &&
               (py >= y - half) && (py <= y + half);
    }
};
