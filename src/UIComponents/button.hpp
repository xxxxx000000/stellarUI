#pragma once
#include "component.hpp"
#include <functional>
#include "../config.hpp"
#include <cmath>

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

    float fw,fh;
    bool flexLayout = false;

    int screenWidth;
    int screenHeight;

    Button(int _x, int _y, int _w, int _h, uint16_t _color,
           ButtonShape _shape = ButtonShape::RoundedRect,
           int _radius = 8,
           uint16_t _pressedColor = TFT_DARKGREY)
        : Component(_x, _y, _color), width(_w), height(_h),
          radius(_radius), shape(_shape), pressedColor(_pressedColor),
          animActive(false), animProgress(0), animStartScale(1.0f) {}
    Button(float _fx, float _fy, float _fw, float _fh, uint16_t _color,
           ButtonShape _shape = ButtonShape::RoundedRect,
           int _radius = 8,
           uint16_t _pressedColor = TFT_DARKGREY)
        : Component(_fx, _fy, _color), fw(_fw), fh(_fh),
          radius(_radius), shape(_shape), pressedColor(_pressedColor),
          animActive(false), animProgress(0), animStartScale(1.0f), flexLayout(true) {}

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;
        screenWidth = buf.width();
        screenHeight = buf.height();
        int scaledW;
        int scaledH;
        int drawX;
        int drawY;

        if(!flexLayout){
            scaledW = width * scale;
            scaledH = height * scale;

            drawX = x + offsetX - scaledW / 2;
            drawY = y + offsetY - scaledH / 2;
        }else{
            scaledW = fw * buf.width() * scale;
            scaledH = fh * buf.height() * scale;

            drawX = fx * buf.width() + offsetX - scaledW / 2;
            drawY = fy * buf.height() + offsetY - scaledH / 2;
        }

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
                buf.fillSmoothCircle(drawX + scaledW / 2,drawY + scaledH / 2, r, drawColor);
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

            float eased = easeOutBack(animProgress);

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

    virtual bool containsPoint(int px, int py) {
        int centerX, centerY;
        int halfW, halfH;

        if (!flexLayout) {
            centerX = x;
            centerY = y;
            halfW = (int)(width * scale) / 2;
            halfH = (int)(height * scale) / 2;
        } else {
            centerX = (int)(fx * screenWidth);
            centerY = (int)(fy * screenHeight);
            halfW = (int)(fw * screenWidth * scale) / 2;
            halfH = (int)(fh * screenHeight * scale) / 2;
        }

        switch (shape) {
            case ButtonShape::Circle: {
                int r = halfW < halfH ? halfW : halfH;
                int dx = px - centerX;
                int dy = py - centerY;
                return (dx * dx + dy * dy) <= (r * r);
            }
            default:
                return (px >= centerX - halfW) && (px <= centerX + halfW) &&
                       (py >= centerY - halfH) && (py <= centerY + halfH);
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
    static constexpr uint32_t animDuration = buttonDuration;

    float easeOutBack(float t) {
        float c1 = 1.2f;
        float c3 = c1 + 1.0f;
        return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);
    }
};
