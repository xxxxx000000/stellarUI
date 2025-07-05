#pragma once
#include "component.h"
#include <functional>

/*
    滑块组件，参数为横纵坐标，长宽，圆角半径，滑动方向（纵向/水平），背景颜色，填充颜色，填充方向（0 = 左→右 或 上→下；1 = 右→左 或 下→上），默认数值
    Slider，with parameters: X and Y coordinates, width, height, corner radius, slide direction (vertical/horizontal), background color, fill color, fill direction (0 = left to right or top to bottom; 1 = right to left or bottom to top), and default value
*/

enum class SliderDirection {
    Horizontal,
    Vertical
};

class Slider : public Component {
public:
    int width, height;
    int radius;
    SliderDirection direction;
    int fillDirection;
    std::function<void(uint8_t)> onChange;

    uint16_t backgroundColor;
    uint16_t fillColor;

private:
    bool dragging = false;
    int value = 0;
    int lastReportedValue = -1;
    int touchPadding = 6;

public:
    Slider(int _x, int _y, int _width, int _height, int _radius,
           SliderDirection _direction = SliderDirection::Horizontal,
           uint16_t _backgroundColor = TFT_DARKGREY,
           uint16_t _fillColor = TFT_WHITE,
           int _fillDirection = 0,
           uint8_t defaultValue = 0)
        : Component(_x, _y),
          width(_width), height(_height),
          radius(_radius),
          direction(_direction),
          backgroundColor(_backgroundColor),
          fillColor(_fillColor),
          fillDirection(_fillDirection)
    {
        value = constrain(defaultValue, 0, 255);
    }

    void draw(TFT_eSprite &buf, int offsetX = 0, int offsetY = 0) override {
        int absX = x + offsetX;
        int absY = y + offsetY;
        buf.fillSmoothRoundRect(absX, absY, width, height, radius, backgroundColor);

        if (value <= 0) return;

        if (direction == SliderDirection::Horizontal) {
            int fillLen = map(value, 0, 255, 0, width);
            int visualFillLen = max(fillLen, radius * 2);
            visualFillLen = min(visualFillLen, width);

            int fx = (fillDirection == 0)
                     ? absX
                     : absX + width - visualFillLen;

            buf.fillSmoothRoundRect(fx, absY, visualFillLen, height, radius, fillColor);

        } else {
            int fillLen = map(value, 0, 255, 0, height);
            int visualFillLen = max(fillLen, radius * 2);
            visualFillLen = min(visualFillLen, height);

            int fy = (fillDirection == 0)
                     ? absY + height - visualFillLen
                     : absY;

            buf.fillSmoothRoundRect(absX, fy, width, visualFillLen, radius, fillColor);
        }
    }

    bool containsPoint(int px, int py) override {
        return px >= x - touchPadding &&
               px <= x + width + touchPadding &&
               py >= y - touchPadding &&
               py <= y + height + touchPadding;
    }

    void handleTouch(int tx, int ty) override {
        if (!enabled) return;
        if (containsPoint(tx, ty)) {
            dragging = true;
            updateValueFromTouch(tx, ty);
        } else {
            dragging = false;
        }
    }

    void handleRelease() override {
        dragging = false;
    }

    void handlePress(int tx, int ty) override {
        if (containsPoint(tx, ty)) {
            dragging = true;
            updateValueFromTouch(tx, ty);
        }
    }

    void update(uint32_t now) override {
        Component::update(now);
        if (dragging && onChange) {
            if (value != lastReportedValue) {
                lastReportedValue = value;
                onChange(value);
            }
        }
    }

    void setValue(uint8_t v) {
        value = constrain(v, 0, 255);
    }

    uint8_t getValue() const {
        return value;
    }

private:
    void updateValueFromTouch(int tx, int ty) {
        int relative;

        if (direction == SliderDirection::Horizontal) {
            relative = constrain(tx - x, 0, width);
            value = map((fillDirection == 0) ? relative : width - relative, 0, width, 0, 255);
        } else {
            relative = constrain(ty - y, 0, height);
            value = map((fillDirection == 0) ? height - relative : relative, 0, height, 0, 255);
        }

        value = constrain(value, 0, 255);
    }
};
