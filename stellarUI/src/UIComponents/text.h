#pragma once
#include "component.h"
#include <TFT_eSPI.h>
#include <Arduino.h>

/*
    文本组件 可以外部使用参数为横纵坐标，内容，字体大小，颜色，是否以横纵坐标为中心（布尔）
    可以使用setText从外部改变文本内容
    Text component that can be used externally, with parameters: X and Y coordinates, content, font size, color, and a boolean indicating whether the coordinates are the center
    The text content can be changed externally using the setText method

*/

class Text : public Component {
public:
    String content;
    int fontSize;
    bool centered;

    Text(int _x, int _y, const String& _content, int _fontSize = 2, uint16_t _color = TFT_WHITE, bool _centered = false)
        : Component(_x, _y, _color), content(_content), fontSize(_fontSize), centered(_centered) {}

    void setText(const String& newText) {
        content = newText;
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        int drawX = x + offsetX;
       int drawY = y + offsetY;

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
