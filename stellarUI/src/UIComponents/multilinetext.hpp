#pragma once
#include "component.hpp"
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
T clamp4(float val, T minVal, T maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

class MultilineText : public Component {
private:
    std::vector<String> inputLines;
    std::vector<String> wrappedLines;

    int width, height, fontSize;
    uint16_t textColor;
    int padding = 6;

    float scrollOffset = 0;
    int maxScroll = 0;
    int lineHeight = 0;

    float fw,fh;
    int drawX,drawY,drawW,drawH;

    int touchStartY = -1;
    float scrollStartOffset = 0;
    bool isTouching = false;

    const float overscrollDamping = 0.5f;
    const float bounceBackSpeed = 0.3f;

    bool flexLayout = false;

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
        int maxWidth = drawW - 2 * padding;

        for (size_t i = 0; i < inputLines.size(); i++) {
            auto partial = wrapSingleLine(buf, inputLines[i], maxWidth);
            wrappedLines.insert(wrappedLines.end(), partial.begin(), partial.end());
            if (i != inputLines.size() - 1) wrappedLines.push_back("");
        }

        lineHeight = buf.fontHeight() + 4;
        int totalHeight = (int)wrappedLines.size() * lineHeight;
        maxScroll = std::max(0, totalHeight - drawH + 2 * padding);
    }

public:
    MultilineText(int _x, int _y, int _width, int _height,
                  int _fontSize = 2, uint16_t _color = TFT_WHITE)
        : Component(_x, _y, _color),
          width(_width), height(_height), fontSize(_fontSize), textColor(_color) {}
    MultilineText(float _fx, float _fy, float _fw, float _fh,
                  int _fontSize = 2, uint16_t _color = TFT_WHITE)
        : Component(_fx, _fy, _color),
          fw(_fw), fh(_fh), fontSize(_fontSize), textColor(_color), flexLayout(true) {}

    void setTextLines(const std::vector<String>& lines) {
        inputLines = lines;
        scrollOffset = 0;
        wrappedLines.clear();
    }
    void setFlexLayoutParams(float _fx, float _fy, float _fw, float _fh) {
        fx = _fx;
        fy = _fy;
        fw = _fw;
        fh = _fh;
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if(!flexLayout){
            drawX = x + offsetX - width / 2;
            drawY = y + offsetY - height / 2;
            drawW = width;
            drawH = height;
        }else{
            drawW = fw * buf.width();
            drawH = fh * buf.height();
            drawX = fx * buf.width() + offsetX - drawW / 2;
            drawY = fy * buf.height() + offsetY - drawH / 2;
        }

        buf.setTextColor(textColor);
        buf.setTextSize(fontSize);
        buf.setTextDatum(TL_DATUM);

        if (wrappedLines.empty()) wrapText(buf);
        if (scrollOffset < 0) {
            scrollOffset += (0 - scrollOffset) * bounceBackSpeed;
            if (scrollOffset > 0) scrollOffset = 0;
        } else if (scrollOffset > maxScroll) {
            scrollOffset += (maxScroll - scrollOffset) * bounceBackSpeed;
            if (scrollOffset < maxScroll) scrollOffset = maxScroll;
        }
        int yOffset = drawY + padding - scrollOffset;
        for (const auto& line : wrappedLines) {
            if (yOffset + lineHeight >= drawY && yOffset <= drawY + drawH) {
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
            float dy = ty - touchStartY;
            float newOffset = scrollStartOffset - dy;
            if (newOffset < 0) {
                newOffset = newOffset * overscrollDamping;
            } else if (newOffset > maxScroll) {
                newOffset = maxScroll + (newOffset - maxScroll) * overscrollDamping;
            }

            scrollOffset = newOffset;
        }
    }

    void onRelease() override {
        isTouching = false;
    }

    bool containsPoint(int px, int py) override {
        return px >= drawX && px <= drawX + drawW &&
               py >= drawY && py <= drawY + drawH;
    }

    void scrollUp(int amount) {
        scrollOffset = clamp4(scrollOffset - amount, 0, maxScroll);
    }

    void scrollDown(int amount) {
        scrollOffset = clamp4(scrollOffset + amount, 0, maxScroll);
    }
};