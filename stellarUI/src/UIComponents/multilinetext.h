#pragma once
#include "component.h"
#include <TFT_eSPI.h>
#include <vector>
#include <Arduino.h>

/*
    适用于内容多的文本，参数为横纵坐标，颜色，字体大小
    使用settextLines设置文本内容，传入一个字符串数组
    appropiate for displaying long text content, with parameters for position, color, and font size
    Use setTextLines to set the text content, passing a vector of strings

*/

template <typename T>
T clamp4(T val, T minVal, T maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

class MultilineText : public Component {
private:
    std::vector<String> inputLines;

    int width;
    int height;
    int fontSize;
    uint16_t textColor;
    int padding = 6;

    int scrollOffset = 0;
    int maxScroll = 0;

    int touchStartY = -1;
    int scrollStartOffset = 0;
    bool isTouching = false;

    std::vector<String> wrappedLines;
    int lineHeight = 0;

    std::vector<String> wrapSingleLine(TFT_eSprite& buf, const String& text, int maxWidth) {
        std::vector<String> result;
        int len = text.length();
        int start = 0;
        while (start < len) {
            int end = start;
            int lastFit = start;
            while (end < len) {
                String sub = text.substring(start, end + 1);
                if (buf.textWidth(sub) > maxWidth) break;
                lastFit = end + 1;
                end++;
            }
            if (lastFit == start) lastFit = start + 1;
            result.push_back(text.substring(start, lastFit));
            start = lastFit;
        }
        return result;
    }

    void wrapText(TFT_eSprite& buf) {
        wrappedLines.clear();
        buf.setTextSize(fontSize);
        int maxWidth = width - 2 * padding;

        for (size_t i = 0; i < inputLines.size(); i++) {
            std::vector<String> partialLines = wrapSingleLine(buf, inputLines[i], maxWidth);
            wrappedLines.insert(wrappedLines.end(), partialLines.begin(), partialLines.end());

            if (i != inputLines.size() - 1) {
                wrappedLines.push_back("");
            }
        }

        lineHeight = buf.fontHeight() + 4;
        int totalHeight = (int)wrappedLines.size() * lineHeight;
        maxScroll = std::max(0, totalHeight - height + padding * 2);
    }

public:
    MultilineText(int _x, int _y, int _width, int _height,
                  int _fontSize = 2, uint16_t _color = TFT_WHITE)
        : Component(_x, _y, _color),
          width(_width), height(_height), fontSize(_fontSize), textColor(_color) {}

    void setTextLines(const std::vector<String>& lines) {
        inputLines = lines;
        scrollOffset = 0;
        wrappedLines.clear();
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        int drawX = x + offsetX - width / 2;
        int drawY = y + offsetY - height / 2;
        buf.setTextColor(textColor);
        buf.setTextSize(fontSize);
        buf.setTextDatum(TL_DATUM);

        if (wrappedLines.empty()) wrapText(buf);

        int yOffset = drawY + padding - scrollOffset;
        for (const String& line : wrappedLines) {
            if (yOffset + lineHeight >= drawY && yOffset <= drawY + height) {
                buf.drawString(line, drawX + padding, yOffset);
            }
            yOffset += lineHeight;
        }
    }

    void handleTouch(int tx, int ty) override {
        if (!enabled || !containsPoint(tx, ty)) {
            isTouching = false;
            return;
        }

        if (!isTouching) {
            isTouching = true;
            touchStartY = ty;
            scrollStartOffset = scrollOffset;
        } else {
            int delta = touchStartY - ty;
            scrollOffset = scrollStartOffset + delta;
            scrollOffset = clamp4(scrollOffset, 0, maxScroll);
        }
    }

    void onRelease() override {
        isTouching = false;
    }

    bool containsPoint(int px, int py) override {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
    void scrollUp(int amount) {
        scrollOffset = clamp4(scrollOffset - amount, 0, maxScroll);
    }
    void scrollDown(int amount) {
        scrollOffset = clamp4(scrollOffset + amount, 0, maxScroll);
    }
};
