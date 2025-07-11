#pragma once
#include "component.h"
#include <functional>
#include <cmath> // 用于fabs

/*
    按钮组件，参数为中心点的横纵坐标，按钮宽高，颜色，形状类型，圆角半径，按下时的颜色（默认为灰）
    可以使用onClickCallback设置按下后会触发的程序
*/

enum class ButtonShape {
    Rectangle,
    RoundedRect,
    Circle
};

class Button : public Component {
public:
    int width, height, radius;
    ButtonShape shape;
    bool pressed = false;
    std::function<void()> onClickCallback;

    uint16_t pressedColor;
    String customEventName;

    Button(int _x, int _y, int _w, int _h, uint16_t _color,
           ButtonShape _shape = ButtonShape::RoundedRect,
           int _radius = 8,
           uint16_t _pressedColor = TFT_DARKGREY)
        : Component(_x, _y, _color), width(_w), height(_h),
          radius(_radius), shape(_shape), pressedColor(_pressedColor),
          animActive(false), animProgress(0), animStartScale(1.0f) {}

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        int scaledW = width * scale;
        int scaledH = height * scale;

        int drawX = x + offsetX - scaledW / 2;
        int drawY = y + offsetY - scaledH / 2;

        uint16_t drawColor = pressed ? pressedColor : color;

        switch (shape) {
            case ButtonShape::Rectangle:
                buf.fillRect(drawX, drawY, scaledW, scaledH, drawColor);
                break;
            case ButtonShape::RoundedRect:
                buf.fillSmoothRoundRect(drawX, drawY, scaledW, scaledH, radius, drawColor);
                break;
            case ButtonShape::Circle: {
                int r = (scaledW < scaledH ? scaledW : scaledH) / 2;
                buf.fillSmoothCircle(x + offsetX, y + offsetY, r, drawColor);
                break;
            }
        }
    }

    virtual void update(uint32_t now) override {
        Component::update(now);

        const float pressedScale = 0.9f;
        const float normalScale = 1.0f;
        const float EPSILON = 0.01f;

        float target = pressed ? pressedScale : normalScale;

        if (fabs(scale - target) > EPSILON) {
            if (!animActive) {
                animActive = true;
                animStartTime = now;
                animStartScale = scale;
            }
            uint32_t elapsed = now - animStartTime;
            animProgress = (float)elapsed / animDuration;
            if (animProgress > 1.0f) animProgress = 1.0f;

            float eased = easeInOut(animProgress);

            scale = animStartScale + (target - animStartScale) * eased;

            if (animProgress >= 1.0f) {
                animActive = false;
                scale = target;
            }
        } else {
            scale = target;
            animActive = false;
        }
    }

    virtual bool containsPoint(int px, int py) override {
        int halfW = (width * scale) / 2;
        int halfH = (height * scale) / 2;

        switch (shape) {
            case ButtonShape::Circle: {
                int r = halfW < halfH ? halfW : halfH;
                int dx = px - x;
                int dy = py - y;
                return (dx * dx + dy * dy) <= (r * r);
            }
            default:
                return (px >= x - halfW) && (px <= x + halfW) &&
                       (py >= y - halfH) && (py <= y + halfH);
        }
    }

    virtual void onPress() override {
        pressed = true;
    }

    virtual void onRelease() override {
        if (pressed) {
            pressed = false;
            onClick();
        }
    }

    virtual void onClick() override {
        if (onClickCallback) onClickCallback();
    }

    virtual void handleTouch(int tx, int ty) override {
        if (!enabled) return;

        bool inside = containsPoint(tx, ty);

        if (inside && !pressed) {
            onPress();
        } else if (!inside && pressed) {
            onRelease();
        }
    }

    virtual String getCustomEventName() const {
        return customEventName;
    }

    virtual void handleEvent(const UIEvent& e) {
        if (e.type == UIEventType::Programmatic && e.customData == getCustomEventName()) {
            onClick();
        }
    }

private:
    bool animActive;
    float animProgress;
    float animStartScale;
    uint32_t animStartTime;
    static constexpr uint32_t animDuration = 400;

    float easeInOut(float t) {
        float c1 = 1.2f;
        float c3 = c1 + 1.0f;
        return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);
    }
};
