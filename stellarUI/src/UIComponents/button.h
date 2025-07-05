#pragma once
#include "component.h"
#include <functional>

/*
    按钮组件，参数为中心点的横纵坐标，按钮宽高，颜色，形状类型，圆角半径，按下时的颜色（默认为灰）
    可以使用onlickcallback设置按下后会触发的程序
    Button component，with parameters: center X and Y coordinates, width, height, color, shape type, corner radius, and pressed color (default is gray)
    An onclickcallback can be set to specify the function triggered when the button is pressed

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
          radius(_radius), shape(_shape), pressedColor(_pressedColor) {}

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
        float target = pressed ? 0.9f : 1.0f;
        targetScale += (target - targetScale) * 0.2f;
        scale = targetScale;
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

        if (containsPoint(tx, ty)) {
            if (!pressed) onPress();
        } else {
            if (pressed) onRelease();
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
};
