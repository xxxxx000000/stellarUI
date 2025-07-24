#pragma once
#include "component.h"
#include "../config.h"
#include <functional>

/*
    Switch 开关组件（胶囊形，中心点定位）
    支持 flex 布局
*/

class Switch : public Component {
public:
    int width;
    int radius;

    float fw;
    bool flexLayout = false;

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
        scale = 1.0f;
        targetScale = 1.0f;
    }
    Switch(float _fx, float _fy,
           float _fw, int _radius = 14,
           bool defaultState = false,
           uint16_t _onColor = TFT_GREEN,
           uint16_t _offColor = TFT_DARKGREY,
           uint16_t _knobColor = TFT_WHITE,
           uint16_t _pressedColor = TFT_LIGHTGREY)
        : Component(_fx, _fy),
          fw(_fw), radius(_radius),
          state(defaultState),
          onColor(_onColor), offColor(_offColor),
          knobColor(_knobColor), pressedColor(_pressedColor),
          flexLayout(true)
    {
        knobPos = state ? 1.0f : 0.0f;
        scale = 1.0f;
        targetScale = 1.0f;
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        computeLayout(buf, offsetX, offsetY);

        uint16_t bgColor = pressed ? pressedColor : (state ? onColor : offColor);
        buf.fillSmoothRoundRect(drawX, drawY, scaledW, scaledH, scaledH / 2, bgColor);

        int knobSize = scaledH - 4;
        int knobX = drawX + 2 + (int)((scaledW - knobSize - 4) * knobPos);

        buf.fillSmoothCircle(knobX + knobSize / 2, drawY + scaledH / 2, knobSize / 2, TFT_LIGHTGREY);
        buf.fillSmoothCircle(knobX + knobSize / 2, drawY + scaledH / 2, knobSize / 2 - 1, knobColor);
    }

    void update(uint32_t now) override {
        Component::update(now);

        if (scaleAnimActive) {
            float elapsed = (now - scaleAnimStartTime) / (float)animDuration;
            if (elapsed >= 1.0f) {
                elapsed = 1.0f;
                scaleAnimActive = false;
            }
            float eased = easeOutBack(elapsed);
            scale = scaleAnimStartScale + (targetScale - scaleAnimStartScale) * eased;
        }

        if (animActive) {
            float elapsed = (now - animStartTime) / (float)animDuration;
            if (elapsed >= 1.0f) {
                elapsed = 1.0f;
                animActive = false;
                knobPos = animTargetKnobPos;
            } else {
                float eased = easeOutBack(elapsed);
                knobPos = animStartKnobPos + (animTargetKnobPos - animStartKnobPos) * eased;
            }
        } else {
            knobPos = state ? 1.0f : 0.0f;
        }
    }

    bool containsPoint(int px, int py) override {
        if (!lastBuf) return false;
        computeLayout(*lastBuf, 0, 0);
        return px >= drawX && px <= drawX + scaledW &&
               py >= drawY && py <= drawY + scaledH;
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
        setValue(!state);
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
    int scaledW, scaledH;
    int drawX, drawY;
    TFT_eSprite* lastBuf = nullptr;

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

    static constexpr uint32_t animDuration = switchDuration;

    float easeOutBack(float t) {
        float c1 = 1.2f;
        float c3 = c1 + 1.0f;
        return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);
    }

    void computeLayout(TFT_eSprite& buf, int offsetX, int offsetY) {
        lastBuf = &buf;

        int h = radius * 2;
        if (flexLayout) {
            scaledW = (int)(fw * buf.width() * scale);
        } else {
            scaledW = (int)(width * scale);
        }
        scaledH = (int)(h * scale);

        int cx = flexLayout ? (int)(fx * buf.width()) : x;
        int cy = flexLayout ? (int)(fy * buf.height()) : y;

        drawX = cx + offsetX - scaledW / 2;
        drawY = cy + offsetY - scaledH / 2;
    }
};
