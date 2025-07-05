#pragma once
#include "component.h"
#include <functional>

/*
    Switch 开关组件（胶囊形，中心点定位）
    用于调控某一布尔值
*/

class Switch : public Component {
public:
    int width;
    int radius;

    bool state = false;
    bool pressed = false;

    uint16_t onColor;
    uint16_t offColor;
    uint16_t knobColor;
    uint16_t pressedColor;

    std::function<void(bool)> onClickCallback;

    Switch(int _x, int _y,
           int _width, int _radius = 14,
           bool defaultState = false,
           uint16_t _onColor = TFT_GREEN,
           uint16_t _offColor = TFT_DARKGREY,
           uint16_t _knobColor = TFT_WHITE,
           uint16_t _pressedColor = TFT_LIGHTGREY)
        : Component(_x, _y),
          width(_width), radius(_radius),
          state(defaultState),
          onColor(_onColor), offColor(_offColor),
          knobColor(_knobColor), pressedColor(_pressedColor) {}

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        int height = radius * 2;
        int scaledW = width * scale;
        int scaledH = height * scale;

        int drawX = x + offsetX - scaledW / 2;
        int drawY = y + offsetY - scaledH / 2;

        uint16_t bgColor = pressed ? pressedColor : (state ? onColor : offColor);
        buf.fillSmoothRoundRect(drawX, drawY, scaledW, scaledH, scaledH / 2, bgColor);

        int knobSize = scaledH - 4;
        int knobX = state
            ? drawX + scaledW - knobSize - 2
            : drawX + 2;

        buf.fillSmoothCircle(knobX + knobSize / 2, drawY + scaledH / 2, knobSize / 2, knobColor);
    }

    void update(uint32_t now) override {
        Component::update(now);
        float target = pressed ? 0.9f : 1.0f;
        targetScale += (target - targetScale) * 0.2f;
        scale = targetScale;
    }

    bool containsPoint(int px, int py) override {
        int height = radius * 2;
        int halfW = (width * scale) / 2;
        int halfH = (height * scale) / 2;
        return px >= x - halfW && px <= x + halfW &&
               py >= y - halfH && py <= y + halfH;
    }

    void onPress() override {
        pressed = true;
    }

    void onRelease() override {
        if (pressed) {
            pressed = false;
            onClick();
        }
    }

    void onClick() override {
        state = !state;
        if (onClickCallback) onClickCallback(state);
    }

    void handleTouch(int tx, int ty) override {
        if (!enabled) return;
        if (containsPoint(tx, ty)) {
            if (!pressed) onPress();
        } else {
            if (pressed) onRelease();
        }
    }

    void setValue(bool v) {
        if (state != v) {
            state = v;
            if (onClickCallback) onClickCallback(state);
        }
    }

    bool getValue() const {
        return state;
    }
};
