#pragma once
#include "Component.h"
#include <vector>
#include <functional>
#include <TFT_eSPI.h>

/*
    线图组件，参数横纵坐标，宽高，lambda表达式
    表达式内为要记录的数据
    提供缩放，停止记录等方法
    // Line chart component with parameters: X and Y coordinates, width, height, and a lambda expression
    // The lambda expression provides the data to be recorded
    // Methods are available for scaling, pausing data recording, and more

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

    SampleFunction sampleFunc;
    bool samplingPaused = false;

    bool showCursor = false;
    int cursorIndex = -1;

    bool dragging = false;
    int lastX = 0, lastY = 0;

    float scaleX = 1.0f;
    float scaleY = 1.0f;

public:
    LineChartComponent(int _x, int _y, int _width, int _height, SampleFunction func)
        : Component(_x, _y), sampleFunc(func), width(_width), height(_height) {}

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

        int drawX = x + offsetX;
        int drawY = y + offsetY;

        buf.fillRect(drawX, drawY, width, height, TFT_BLACK);
        buf.drawRect(drawX, drawY, width, height, color);

        float pointSpacing = scaleX;
        int visiblePoints = (int)(width / pointSpacing);
        int maxDataOffset = std::max((int)(data.size() * pointSpacing) - width, 0);
        int clampedViewOffset = clamp(viewOffset, 0, maxDataOffset);
        int startIndex = (int)((data.size() * pointSpacing - width - clampedViewOffset) / pointSpacing);
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

            int y1 = drawY + height - (int)(((v1 - scaledMinY + verticalOffset) / (scaledMaxY - scaledMinY)) * height);
            int y2 = drawY + height - (int)(((v2 - scaledMinY + verticalOffset) / (scaledMaxY - scaledMinY)) * height);

            x1 = clamp(x1, drawX, drawX + width - 1);
            x2 = clamp(x2, drawX, drawX + width - 1);
            y1 = clamp(y1, drawY, drawY + height - 1);
            y2 = clamp(y2, drawY, drawY + height - 1);

            buf.drawWideLine(x1, y1, x2, y2,2,TFT_CYAN);
        }

        if (showCursor && cursorIndex >= startIndex && cursorIndex < endIndex) {
            int cx = drawX + (int)((cursorIndex - startIndex) * pointSpacing);
            float cyVal = data[cursorIndex];
            int cy = drawY + height - (int)(((cyVal - scaledMinY + verticalOffset) / (scaledMaxY - scaledMinY)) * height);

            cx = clamp(cx, drawX, drawX + width - 1);
            cy = clamp(cy, drawY, drawY + height - 1);

            buf.drawLine(cx, drawY, cx, drawY + height - 1, TFT_RED);
            buf.fillCircle(cx, cy, 2, TFT_YELLOW);

            char label[16];
            sprintf(label, "%.2f", cyVal);
            buf.setTextColor(TFT_WHITE, TFT_BLACK);
            buf.drawString(label, cx + 4, clamp(cy - 10, drawY, drawY + height - 10));
        }
    }

    bool containsPoint(int px, int py) override {
        return (px >= x && px <= x + width && py >= y && py <= y + height);
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

                int maxDataOffset = std::max((int)(data.size() * scaleX) - width, 0);
                viewOffset = clamp(viewOffset + dx, 0, maxDataOffset);

                verticalOffset -= (float)dy / height * (maxY - minY) / scaleY;
            }
        } else {
            dragging = false;
            showCursor = true;
            if (containsPoint(lastX, lastY)) {
                float pointSpacing = scaleX;
                int visiblePoints = (int)(width / pointSpacing);
                int maxDataOffset = std::max((int)(data.size() * pointSpacing) - width, 0);
                int clampedViewOffset = clamp(viewOffset, 0, maxDataOffset);
                int startIndex = (int)((data.size() * pointSpacing - width - clampedViewOffset) / pointSpacing);
                startIndex = clamp(startIndex, 0, (int)data.size());
                int relativeX = lastX - x;
                cursorIndex = startIndex + (int)(relativeX / pointSpacing);
            }
        }
    }

    // ====== 控制接口 ======
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
        int maxDataOffset = std::max((int)(data.size() * scaleX) - width, 0);
        viewOffset = clamp(viewOffset + 10, 0, maxDataOffset);
    }

    void scrollRight() {
        int maxDataOffset = std::max((int)(data.size() * scaleX) - width, 0);
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
