#pragma once
#include "component.h"
#include <functional>

/*
    适用于动态的文本，参数为横纵坐标，颜色，lambda表达式，字体大小
    表达式内为要显示的文本
    Suitable for dynamic text display, with parameters: X and Y coordinates, color, lambda expression, and font size
    The lambda expression provides the text to be displayed

*/

class LiveText : public Component {
private:
    std::function<String()> textProvider;
    String lastRenderedText;
    int textSize;
    int anchorX, anchorY;

public:
    LiveText(int _x, int _y, uint16_t _color, std::function<String()> _provider, int _textSize = 2)
        : Component(_x, _y, _color), textProvider(_provider), textSize(_textSize) {}

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        String newText = textProvider ? textProvider() : "";

        lastRenderedText = newText;

        buf.setTextColor(color);
        buf.setTextSize(textSize);
        buf.setCursor(x + offsetX, y + offsetY);
        buf.drawString(newText, x + offsetX, y + offsetY);
    }

    void update(uint32_t now) override {
        Component::update(now);
    }

    bool containsPoint(int px, int py) override {
        return false;
    }
};
