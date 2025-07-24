#pragma once
#include "Component.h"
#include <vector>
#include <functional>
#include <TFT_eSPI.h>

/*
    线图组件，参数横纵坐标，宽高，lambda表达式
    表达式内为要记录的数据
    提供缩放，停止记录等方法
    Line chart component with parameters: X and Y coordinates, width, height, and a lambda expression
    The lambda expression provides the data to be recorded
    Methods are available for scaling, pausing data recording, and more

*/

template <typename T>
T clamp(T val, T minVal, T maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

class LineChartComponent : public Component {
public:
    using SampleFunction = std::function<float(void)>;

private:
    std::vector<float> data;
    int maxSamples = 10000;
    float minY = -10.0f, maxY = 10.0f;
    float verticalOffset = 0.0f;
    uint32_t lastSampleTime = 0;
    uint16_t sampleInterval = 100;

    int viewOffset = 0;

    int width = 120, height = 60;
    float fw,fh;
    int cornerRadius = 0;

    SampleFunction sampleFunc;
    bool samplingPaused = false;

    bool showCursor = false;
    int cursorIndex = -1;

    bool dragging = false;
    int lastX = 0, lastY = 0;

    
    int drawX;
    int drawY;
    int drawW;
    int drawH;

    float scaleX = 1.0f;
    float scaleY = 1.0f;
    uint16_t bgColor = TFT_BLACK;
    uint16_t lineColor = TFT_CYAN;
    uint16_t borderColor = TFT_WHITE;
    uint16_t cursorLineColor = TFT_RED;
    uint16_t cursorDotColor = TFT_YELLOW;
    uint16_t cursorTextColor = TFT_WHITE;

    bool flexLayout = false;

public:
    LineChartComponent(
        int _x, int _y, int _width, int _height, SampleFunction func,
        int _cornerRadius = 0,
        uint16_t _bgColor = TFT_BLACK,
        uint16_t _lineColor = TFT_CYAN,
        uint16_t _borderColor = TFT_WHITE,
        uint16_t _cursorLineColor = TFT_RED,
        uint16_t _cursorDotColor = TFT_YELLOW,
        uint16_t _cursorTextColor = TFT_WHITE
    )
        : Component(_x, _y),
          sampleFunc(func),
          width(_width), height(_height),
          cornerRadius(_cornerRadius),
          bgColor(_bgColor),
          lineColor(_lineColor),
          borderColor(_borderColor),
          cursorLineColor(_cursorLineColor),
          cursorDotColor(_cursorDotColor),
          cursorTextColor(_cursorTextColor) {}

    LineChartComponent(
        float _fx, float _fy, float _fw, float _fh, SampleFunction func,
        int _cornerRadius = 0,
        uint16_t _bgColor = TFT_BLACK,
        uint16_t _lineColor = TFT_CYAN,
        uint16_t _borderColor = TFT_WHITE,
        uint16_t _cursorLineColor = TFT_RED,
        uint16_t _cursorDotColor = TFT_YELLOW,
        uint16_t _cursorTextColor = TFT_WHITE
    )
        : Component(_fx, _fy),
          sampleFunc(func),
          fw(_fw), fh(_fh),
          cornerRadius(_cornerRadius),
          bgColor(_bgColor),
          lineColor(_lineColor),
          borderColor(_borderColor),
          cursorLineColor(_cursorLineColor),
          cursorDotColor(_cursorDotColor),
          cursorTextColor(_cursorTextColor), flexLayout(true) {}

    void update(uint32_t now) override {
        Component::update(now);
        if (samplingPaused) return;
        if (now - lastSampleTime >= sampleInterval) {
            lastSampleTime = now;
            if (sampleFunc) {
                float value = sampleFunc();
                data.push_back(value);
                if ((int)data.size() > maxSamples)
                    data.erase(data.begin());
            }
        }
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        if(!flexLayout){
            drawX = x + offsetX;
            drawY = y + offsetY;
            drawW = width;
            drawH = height;
        }else{
            drawX = fx * buf.width() + offsetX;
            drawY = fy * buf.height() + offsetY;
            drawW = fw * buf.width();
            drawH = fh * buf.height();
        }

        if (cornerRadius > 0)
            buf.fillSmoothRoundRect(drawX, drawY, drawW, drawH, cornerRadius, bgColor);
        else
            buf.fillRect(drawX, drawY, drawW, drawH, bgColor);

        if (cornerRadius > 0)
            buf.drawSmoothRoundRect(drawX, drawY, cornerRadius, cornerRadius - 1,drawW, drawH, borderColor, bgColor);
        else
            buf.drawRect(drawX, drawY, drawW, drawH, borderColor);

        float pointSpacing = scaleX;
        int visiblePoints = (int)(drawW / pointSpacing);
        int maxDataOffset = std::max((int)(data.size() * pointSpacing) - drawW, 0);
        int clampedViewOffset = clamp(viewOffset, 0, maxDataOffset);
        int startIndex = (int)((data.size() * pointSpacing - drawW - clampedViewOffset) / pointSpacing);
        startIndex = clamp(startIndex, 0, (int)data.size());
        int endIndex = clamp(startIndex + visiblePoints, 0, (int)data.size());

        if (endIndex - startIndex < 2) return;

        float centerY = (minY + maxY) / 2.0f;
        float halfRange = (maxY - minY) / 2.0f / scaleY;
        float scaledMinY = centerY - halfRange;
        float scaledMaxY = centerY + halfRange;

        for (int i = startIndex + 1; i < endIndex; ++i) {
            float v1 = data[i - 1];
            float v2 = data[i];

            int x1 = drawX + (int)((i - 1 - startIndex) * pointSpacing);
            int x2 = drawX + (int)((i - startIndex) * pointSpacing);

            int y1 = drawY + drawH - (int)(((v1 - scaledMinY + verticalOffset) / (scaledMaxY - scaledMinY)) * drawH);
            int y2 = drawY + drawH - (int)(((v2 - scaledMinY + verticalOffset) / (scaledMaxY - scaledMinY)) * drawH);

            x1 = clamp(x1, drawX, drawX + drawW - 1);
            x2 = clamp(x2, drawX, drawX + drawW - 1);
            y1 = clamp(y1, drawY, drawY + drawH - 1);
            y2 = clamp(y2, drawY, drawY + drawH - 1);

            buf.drawWideLine(x1, y1, x2, y2,2, lineColor);
        }

        if (showCursor && cursorIndex >= startIndex && cursorIndex < endIndex) {
            int cx = drawX + (int)((cursorIndex - startIndex) * pointSpacing);
            float cyVal = data[cursorIndex];
            int cy = drawY + drawH - (int)(((cyVal - scaledMinY + verticalOffset) / (scaledMaxY - scaledMinY)) * drawH);

            cx = clamp(cx, drawX, drawX + drawW - 1);
            cy = clamp(cy, drawY, drawY + drawH - 1);

            buf.drawWideLine(cx, drawY, cx, drawY + drawH - 1, 2, cursorLineColor);
            buf.fillSmoothCircle(cx, cy, 2, cursorDotColor);

            char label[16];
            sprintf(label, "%.2f", cyVal);
            buf.setTextColor(cursorTextColor, bgColor);
            buf.drawString(label, cx + 4, clamp(cy - 10, drawY, drawY + drawH - 10));
        }
    }

    bool containsPoint(int px, int py) override {
        return (px >= drawX && px <= drawX + drawW &&
            py >= drawY && py <= drawY + drawH);
    }

    void handleTouch(int tx, int ty) override {
        if (!enabled) return;

        if (tx >= 0 && ty >= 0) {
            if (!dragging && containsPoint(tx, ty)) {
                dragging = true;
                lastX = tx;
                lastY = ty;
                showCursor = false;
            } else if (dragging) {
                int dx = tx - lastX;
                int dy = ty - lastY;
                lastX = tx;
                lastY = ty;

                int maxDataOffset = std::max((int)(data.size() * scaleX) - drawW, 0);
                viewOffset = clamp(viewOffset + dx, 0, maxDataOffset);

                verticalOffset -= (float)dy / drawH * (maxY - minY) / scaleY;
            }
        } else {
            dragging = false;
            showCursor = true;
            if (containsPoint(lastX, lastY)) {
                float pointSpacing = scaleX;
                int visiblePoints = (int)(drawW / pointSpacing);
                int maxDataOffset = std::max((int)(data.size() * pointSpacing) - drawW, 0);
                int clampedViewOffset = clamp(viewOffset, 0, maxDataOffset);
                int startIndex = (int)((data.size() * pointSpacing - drawW - clampedViewOffset) / pointSpacing);
                startIndex = clamp(startIndex, 0, (int)data.size());
                int relativeX = lastX - x;
                cursorIndex = startIndex + (int)(relativeX / pointSpacing);
            }
        }
    }

    void setRange(float minVal, float maxVal) {
        minY = minVal;
        maxY = maxVal;
    }

    void setSampleInterval(uint16_t ms) {
        sampleInterval = ms;
    }

    void setSampleFunc(SampleFunction func) {
        sampleFunc = func;
    }

    void resetData() {
        data.clear();
        viewOffset = 0;
        verticalOffset = 0;
        scaleX = 1.0f;
        scaleY = 1.0f;
        cursorIndex = -1;
        showCursor = false;
    }

    void scrollLeft() {
        int maxDataOffset = std::max((int)(data.size() * scaleX) - drawW, 0);
        viewOffset = clamp(viewOffset + 10, 0, maxDataOffset);
    }

    void scrollRight() {
        int maxDataOffset = std::max((int)(data.size() * scaleX) - drawW, 0);
        viewOffset = clamp(viewOffset - 10, 0, maxDataOffset);
    }

    void scrollUp() {
        verticalOffset -= (maxY - minY) * 0.1f / scaleY;
    }

    void scrollDown() {
        verticalOffset += (maxY - minY) * 0.1f / scaleY;
    }

    void zoomIn() {
        scaleX *= 1.1f;
        scaleY *= 1.1f;
        if (scaleX > 10.0f) scaleX = 10.0f;
        if (scaleY > 10.0f) scaleY = 10.0f;
    }

    void zoomOut() {
        scaleX /= 1.1f;
        scaleY /= 1.1f;
        if (scaleX < 0.3f) scaleX = 0.3f;
        if (scaleY < 0.3f) scaleY = 0.3f;
    }

    void pauseSampling() { samplingPaused = true; }
    void resumeSampling() { samplingPaused = false; }
    bool isSamplingPaused() const { return samplingPaused; }
};
