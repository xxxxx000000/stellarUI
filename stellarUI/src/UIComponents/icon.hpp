#pragma once
#include <TFT_eSPI.h>
#include "component.hpp"
#include <functional>

/*
    图标组件，允许通过回调绘制自定义图标
    Icon component that uses a draw callback for custom rendering
*/

class IconComponent : public Component {
private:
    int size = 32;
    std::function<void(TFT_eSprite&, int centerX, int centerY, float scale)> drawCallback;
    int screenWidth,screenHeight;
    bool flexLayout = false;

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
    IconComponent(float _fx, float _fy,
                  std::function<void(TFT_eSprite&, int, int, float)> _drawCallback,
                  uint16_t _color = TFT_WHITE)
        : Component(_fx, _fy, _color), drawCallback(_drawCallback) {
        targetX = x;
        targetY = y;
        targetScale = scale = 1.0f;
        enabled = true;
        flexLayout = true;
    }

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled || !drawCallback) return;

        screenWidth = buf.width();
        screenHeight = buf.height();
        int drawX;
        int drawY;
        if(!flexLayout){
            drawX = x + offsetX;
            drawY = y + offsetY;
        }else{
            drawX = fx * screenWidth + offsetX;
            drawY = fy * screenHeight + offsetY;
        }
        

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
