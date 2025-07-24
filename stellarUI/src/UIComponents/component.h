#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "../UIEvent.h"

/*
    所有UI组件的基类，请勿直接调用
    Base class for all UI components; do not call directly
*/

class Component {
public:
    int x, y;
    float scale = 1.0f;
    float targetScale = 1.0f;
    int targetX,targetY;
    uint16_t color;
    bool enabled = true;

    float fx,fy,targetFX,targetFY;

    Component(int _x = 0, int _y = 0, uint16_t _color = TFT_WHITE)
        : x(_x), y(_y), color(_color) {}
    Component(float _fx, float _fy, uint16_t _color = TFT_WHITE)
        : fx(_fx), fy(_fy), color(_color) {}

    virtual ~Component() {}

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) = 0;
    virtual void update(uint32_t now) {
        scale += (targetScale - scale) * 0.2f;
    }

    virtual bool containsPoint(int px, int py) = 0;
    virtual void onPress() {}
    virtual void onRelease() {}
    virtual void onClick() {}
    virtual void handleTouch(int tx, int ty) {
        if (!enabled) return;
        bool inside = containsPoint(tx, ty);
        if (inside) {
            onPress();
        } else {
            onRelease();
        }
    }
    virtual void handlePress(int x, int y) {}
    virtual void handleRelease() {}
    virtual void handleEvent(const UIEvent& event) {}
    virtual String getCustomEventName() const { return ""; }
};
