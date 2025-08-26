#pragma once
#include "component.hpp"
#include <TFT_eSPI.h>
#include <Arduino.h>

/*
    文本组件：支持横纵坐标为像素或比例
    Text component: support for absolute or relative coordinates (constructor overload)
*/

class Text : public Component {
public:
    String content;
    int fontSize;
    bool centered;

    bool useRelative = false;

    Text(int _x, int _y, const String& _content, int _fontSize = 2, uint16_t _color = TFT_WHITE, bool _centered = false)
        : Component(_x, _y, _color), content(_content), fontSize(_fontSize), centered(_centered) {}

    Text(float _fx, float _fy, const String& _content, int _fontSize = 2, uint16_t _color = TFT_WHITE, bool _centered = false)
        : Component(_fx, _fy, _color), content(_content), fontSize(_fontSize), centered(_centered), useRelative(true) {}

    void setText(const String& newText) {
        content = newText;
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        int drawX, drawY;

        if (useRelative) {
            drawX = buf.width() * fx;
            drawY = buf.height() * fy;
        } else {
            drawX = x;
            drawY = y;
        }

        drawX += offsetX;
        drawY += offsetY;

        buf.setTextColor(color);
        buf.setTextSize(fontSize);
        buf.setTextDatum(centered ? MC_DATUM : TL_DATUM);
        buf.drawString(content, drawX, drawY);
    }

    void update(uint32_t now) override {
        Component::update(now);
    }

    bool containsPoint(int px, int py) override {
        return false;
    }
};
