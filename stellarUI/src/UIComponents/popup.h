#pragma once
#include "component.h"
#include <functional>
#include <Arduino.h>

/*
    弹窗组件，支持弹出式提示框，支持动画效果
    Popup Component that supports pop-up notifications with animation effects
*/

class PopupComponent : public Component {
private:
    int popupHeight;
    int cornerRadius;
    uint16_t bgColor;
    int width;

    float animProgress = 0.0f;
    bool animShowing = false;
    bool animHiding = false;
    bool currentlyVisible = false;
    bool triggeredOnce = false;

    uint32_t animStartTime = 0;
    static constexpr uint32_t animDuration = 300;

    std::function<bool()> isVisibleFunc;
    std::function<String()> getTextFunc;
    std::function<void()> onClickCallback;
    std::function<void()> onDismissCallback;

    float easeOutQuad(float t) {
        return t < 0.5 ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
    }

public:
    PopupComponent(int screenWidth, int height,
                   std::function<bool()> visibleLambda,
                   std::function<String()> textLambda,
                   uint16_t bgColor_ = TFT_WHITE,
                   int cornerRadius_ = 8)
        : popupHeight(height),
          cornerRadius(cornerRadius_),
          bgColor(bgColor_),
          isVisibleFunc(visibleLambda),
          getTextFunc(textLambda) {
        x = 0;
        y = 0;
        width = screenWidth - 20;
        height = popupHeight;
        enabled = true;
    }

    void setOnClick(std::function<void()> cb) {
        onClickCallback = cb;
    }

    void setOnDismiss(std::function<void()> cb) {
        onDismissCallback = cb;
    }

    void update(uint32_t now) override {
        bool shouldBeVisible = isVisibleFunc ? isVisibleFunc() : false;

        if (shouldBeVisible && !triggeredOnce && !currentlyVisible) {
            animShowing = true;
            animHiding = false;
            animStartTime = now;
            currentlyVisible = true;
            triggeredOnce = true;
        }

        if (animShowing) {
            uint32_t elapsed = now - animStartTime;
            float t = elapsed / (float)animDuration;
            animProgress = (t >= 1.0f) ? 1.0f : t;
            if (t >= 1.0f) animShowing = false;
        } else if (animHiding) {
            uint32_t elapsed = now - animStartTime;
            float t = elapsed / (float)animDuration;
            animProgress = (t >= 1.0f) ? 0.0f : 1.0f - t;
            if (t >= 1.0f) {
                animHiding = false;
                currentlyVisible = false;
                triggeredOnce = false;
                if (onDismissCallback) onDismissCallback();
            }
        }
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (animProgress <= 0.0f) return;

        int scaledW = (int)(width * animProgress);
        int scaledH = (int)(popupHeight * animProgress);

        int centerX = offsetX + buf.width() / 2;
        int startY = offsetY - scaledH;
        int endY = offsetY + 10;

        float easedT = easeOutQuad(animProgress);
        int drawY = (int)(startY + (endY - startY) * easedT);
        int drawX = centerX - scaledW / 2;

        buf.fillSmoothRoundRect(drawX - 1, drawY - 1, scaledW + 2, scaledH + 2, cornerRadius, TFT_LIGHTGREY);
        buf.fillSmoothRoundRect(drawX, drawY, scaledW, scaledH, cornerRadius, bgColor);

        if (getTextFunc) {
            String text = getTextFunc();
            buf.setTextColor(TFT_BLACK);
            buf.setTextDatum(TL_DATUM);
            buf.setTextSize(2);

            int textX = drawX + 10;
            int textY = drawY + 10;
            int maxWidth = scaledW - 20;
            int lineHeight = buf.fontHeight() + 4;
            int cursorY = textY;

            int start = 0;
            int len = text.length();

            while (start < len && (cursorY + lineHeight) <= (drawY + scaledH - 10)) {
                int end = start;
                int lastFit = start;
                while (end < len) {
                    String line = text.substring(start, end + 1);
                    int16_t w = buf.textWidth(line);
                    if (w > maxWidth) break;
                    lastFit = end + 1;
                    end++;
                }
                if (lastFit == start) lastFit = start + 1;

                String line = text.substring(start, lastFit);
                buf.drawString(line, textX, cursorY);

                cursorY += lineHeight;
                start = lastFit;
            }
        }
    }

    bool containsPoint(int px, int py) {
        if (animProgress <= 0.0f) return false;

        int scaledW = (int)(width * animProgress);
        int scaledH = (int)(popupHeight * animProgress);
        int centerX = bufWidth() / 2;

        int startY = -scaledH;
        int endY = 10;
        float easedT = easeOutQuad(animProgress);
        int drawY = (int)(startY + (endY - startY) * easedT);
        int drawX = centerX - scaledW / 2;

        return px >= drawX && px <= (drawX + scaledW) &&
               py >= drawY && py <= (drawY + scaledH);
    }

    void handleTouch(int tx, int ty) override {
        if (animProgress <= 0.0f) return;
        if (tx < 0 || ty < 0) return;

        if (containsPoint(tx, ty)) {
            if (onClickCallback) onClickCallback();
        } else {
            if (!animHiding) {
                animHiding = true;
                animShowing = false;
                animStartTime = millis();
            }
            if (onDismissCallback) onDismissCallback();
        }
    }

    int bufWidth() const { return width + 20; }
    int bufHeight() const { return popupHeight + 100; }
};
