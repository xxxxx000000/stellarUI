#pragma once
#include "component.h"
#include <functional>

/*
    Switch 开关组件（胶囊形，中心点定位）
    用于调控某一布尔值，带非线性缩放动画和平滑滑块移动动画
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
          knobColor(_knobColor), pressedColor(_pressedColor)
    {
        knobPos = state ? 1.0f : 0.0f;
        animActive = false;
        scaleAnimActive = false;
        scale = 1.0f;
        targetScale = 1.0f;
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        int height = radius * 2;
        int scaledW = (int)(width * scale);
        int scaledH = (int)(height * scale);

        int drawX = x + offsetX - scaledW / 2;
        int drawY = y + offsetY - scaledH / 2;

        uint16_t bgColor = pressed ? pressedColor : (state ? onColor : offColor);
        buf.fillSmoothRoundRect(drawX, drawY, scaledW, scaledH, scaledH / 2, bgColor);

        int knobSize = scaledH - 4;

        int knobX = drawX + 2 + (int)((scaledW - knobSize - 4) * knobPos);

        buf.fillSmoothCircle(knobX + knobSize / 2, drawY + scaledH / 2, knobSize / 2, knobColor);
    }

    void update(uint32_t now) override {
        Component::update(now);

        if (scaleAnimActive) {
            float elapsed = (now - scaleAnimStartTime) / (float)animDuration;
            if (elapsed >= 1.0f) {
                elapsed = 1.0f;
                scaleAnimActive = false;
            }
            float eased = easeInOut(elapsed);
            scale = scaleAnimStartScale + (targetScale - scaleAnimStartScale) * eased;
        }

        if (animActive) {
            float elapsed = (now - animStartTime) / (float)animDuration;
            if (elapsed >= 1.0f) {
                elapsed = 1.0f;
                animActive = false;
                knobPos = animTargetKnobPos;
            } else {
                float eased = easeInOut(elapsed);
                knobPos = animStartKnobPos + (animTargetKnobPos - animStartKnobPos) * eased;
            }
        } else {
            knobPos = state ? 1.0f : 0.0f;
        }
    }

    bool containsPoint(int px, int py) override {
        int height = radius * 2;
        int halfW = (int)((width * scale) / 2);
        int halfH = (int)((height * scale) / 2);
        return px >= x - halfW && px <= x + halfW &&
               py >= y - halfH && py <= y + halfH;
    }

    void onPress() override {
        pressed = true;
        startScaleAnimation(0.9f);
    }

    void onRelease() override {
        if (pressed) {
            pressed = false;
            startScaleAnimation(1.0f);
            onClick();
        }
    }

    void onClick() override {
        bool newState = !state;
        setValue(newState);
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
            animStartKnobPos = knobPos;
            animTargetKnobPos = state ? 1.0f : 0.0f;
            animStartTime = millis();
            animActive = true;

            if (onClickCallback) onClickCallback(state);
        }
    }

    bool getValue() const {
        return state;
    }

private:
    bool scaleAnimActive = false;
    float scaleAnimStartScale = 1.0f;
    float targetScale = 1.0f;
    uint32_t scaleAnimStartTime = 0;

    void startScaleAnimation(float target) {
        scaleAnimStartScale = scale;
        targetScale = target;
        scaleAnimStartTime = millis();
        scaleAnimActive = true;
    }

    bool animActive = false;
    float animStartKnobPos = 0.0f;
    float animTargetKnobPos = 0.0f;
    uint32_t animStartTime = 0;

    float knobPos = 0.0f;

    static constexpr uint32_t animDuration = 200; // 动画时长，单位ms

    float easeInOut(float t) {
        return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
    }
};
