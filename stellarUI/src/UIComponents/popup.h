#pragma once
#include "component.h"
#include <functional>
#include <Arduino.h>
#include <TFT_eSPI.h>

class PopupComponent : public Component {
private:
    int popupHeight;
    int cornerRadius;
    uint16_t bgColor;
    int width;
    float alpha;
    bool enableBlur = true;

    float animProgress = 0.0f;
    bool animShowing = false;
    bool animHiding = false;
    bool currentlyVisible = false;
    bool triggeredOnce = false;
    bool awaitingMaskReady = false;

    float maskAlpha = 0.0f;

    uint32_t animStartTime = 0;
    static constexpr uint32_t animDuration = 400;

    std::function<bool()> isVisibleFunc;
    std::function<String()> getTextFunc;
    std::function<void()> onClickCallback;
    std::function<void()> onDismissCallback;

    TFT_eSprite* maskSprite = nullptr;
    TFT_eSprite* smallBuf = nullptr;

    static constexpr int downscaleFactor = 1;

    float easeOutQuad(float t) {
        float c1 = 1.2f;
        float c3 = c1 + 1.0f;
        return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);
    }

    void generateBlurMask(TFT_eSprite& buf) {
        int sw = buf.width() / downscaleFactor;
        int sh = buf.height() / downscaleFactor;

        if (smallBuf) {
            smallBuf->deleteSprite();
            delete smallBuf;
            smallBuf = nullptr;
        }
        if (maskSprite) {
            maskSprite->deleteSprite();
            delete maskSprite;
            maskSprite = nullptr;
        }

        smallBuf = new TFT_eSprite(&buf);
        smallBuf->setColorDepth(16);
        smallBuf->createSprite(sw, sh);
        smallBuf->fillSprite(TFT_BLACK);

        // Downscale
        for (int y = 0; y < sh; ++y) {
            for (int x = 0; x < sw; ++x) {
                int sx = x * downscaleFactor;
                int sy = y * downscaleFactor;
                smallBuf->drawPixel(x, y, buf.readPixel(sx, sy));
            }
        }

        // Box blur
        for (int y = 1; y < sh - 1; ++y) {
            for (int x = 1; x < sw - 1; ++x) {
                int r = 0, g = 0, b = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        uint16_t pix = smallBuf->readPixel(x + dx, y + dy);
                        r += (pix >> 11) & 0x1F;
                        g += (pix >> 5) & 0x3F;
                        b += pix & 0x1F;
                    }
                }
                r /= 9; g /= 9; b /= 9;
                smallBuf->drawPixel(x, y, (r << 11) | (g << 5) | b);
            }
        }

        // Upscale using bilinear interpolation
        int dw = buf.width(), dh = buf.height();
        maskSprite = new TFT_eSprite(&buf);
        maskSprite->setColorDepth(16);
        maskSprite->createSprite(dw, dh);

        for (int y = 0; y < dh; ++y) {
            float sy = y / (float)downscaleFactor;
            int y0 = floor(sy);
            int y1 = min(y0 + 1, sh - 1);
            float fy = sy - y0;

            for (int x = 0; x < dw; ++x) {
                float sx = x / (float)downscaleFactor;
                int x0 = floor(sx);
                int x1 = min(x0 + 1, sw - 1);
                float fx = sx - x0;

                auto get = [&](int xx, int yy) -> uint16_t {
                    return smallBuf->readPixel(xx, yy);
                };

                uint16_t c00 = get(x0, y0);
                uint16_t c10 = get(x1, y0);
                uint16_t c01 = get(x0, y1);
                uint16_t c11 = get(x1, y1);

                auto interp = [&](uint16_t c0, uint16_t c1, float t) -> uint16_t {
                    uint8_t r0 = ((c0 >> 11) & 0x1F) << 3;
                    uint8_t g0 = ((c0 >> 5) & 0x3F) << 2;
                    uint8_t b0 = (c0 & 0x1F) << 3;

                    uint8_t r1 = ((c1 >> 11) & 0x1F) << 3;
                    uint8_t g1 = ((c1 >> 5) & 0x3F) << 2;
                    uint8_t b1 = (c1 & 0x1F) << 3;

                    uint8_t r = r0 * (1 - t) + r1 * t;
                    uint8_t g = g0 * (1 - t) + g1 * t;
                    uint8_t b = b0 * (1 - t) + b1 * t;

                    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
                };

                uint16_t top = interp(c00, c10, fx);
                uint16_t bot = interp(c01, c11, fx);
                uint16_t final = interp(top, bot, fy);

                maskSprite->drawPixel(x, y, final);
            }
        }

        smallBuf->deleteSprite();
        delete smallBuf;
        smallBuf = nullptr;

        awaitingMaskReady = false;
        animShowing = true;
        animStartTime = millis();
    }

public:
    PopupComponent(int screenWidth, int height,
                   std::function<bool()> visibleLambda,
                   std::function<String()> textLambda,
                   uint16_t bgColor_ = TFT_WHITE,
                   int cornerRadius_ = 8,
                   float alpha_ = 0.9f, bool enableBlur_ = true)
        : popupHeight(height),
          cornerRadius(cornerRadius_),
          bgColor(bgColor_),
          isVisibleFunc(visibleLambda),
          getTextFunc(textLambda),
          alpha(alpha_), enableBlur(enableBlur_) {
        x = 0;
        y = 0;
        width = screenWidth - 20;
        height = popupHeight;
        enabled = true;
    }

    ~PopupComponent() {
        if (maskSprite) {
            maskSprite->deleteSprite();
            delete maskSprite;
        }
    }

    void setOnClick(std::function<void()> cb) { onClickCallback = cb; }
    void setOnDismiss(std::function<void()> cb) { onDismissCallback = cb; }

    void update(uint32_t now) override {
        bool shouldBeVisible = isVisibleFunc ? isVisibleFunc() : false;

        if (shouldBeVisible && !triggeredOnce && !currentlyVisible && !awaitingMaskReady) {
            awaitingMaskReady = true;
            currentlyVisible = true;
            triggeredOnce = true;
            maskAlpha = 0.0f;
        }

        if (awaitingMaskReady) return;

        if (animShowing) {
            uint32_t elapsed = now - animStartTime;
            float t = elapsed / (float)animDuration;
            if (t >= 1.0f) {
                animProgress = 1.0f;
                maskAlpha = 1.0f;
                animShowing = false;
            } else {
                animProgress = t;
                maskAlpha = easeOutQuad(t);
            }
        } else if (animHiding) {
            uint32_t elapsed = now - animStartTime;
            float t = elapsed / (float)animDuration;
            if (t >= 1.0f) {
                animProgress = 0.0f;
                maskAlpha = 0.0f;
                animHiding = false;
                currentlyVisible = false;
                triggeredOnce = false;
                if (maskSprite) {
                    maskSprite->deleteSprite();
                    delete maskSprite;
                    maskSprite = nullptr;
                }
                if (onDismissCallback) onDismissCallback();
            } else {
                animProgress = 1.0f - t;
                maskAlpha = easeOutQuad(animProgress);
            }
        }
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (awaitingMaskReady) {
            generateBlurMask(buf);
            return;
        }

        if (animProgress <= 0.0f) return;

        if (enableBlur && maskSprite) {
            buf.pushImage(offsetX, offsetY, buf.width(), buf.height(), (uint16_t*)maskSprite->getPointer());
        }

        float easedT = easeOutQuad(animProgress);
        float scale = 0.2f + 0.8f * easedT;

        int baseW = width;
        int baseH = popupHeight;
        int scaledW = (int)(baseW * scale);
        int scaledH = (int)(baseH * scale);

        int centerX = offsetX + buf.width() / 2;
        int baseY = (int)((-baseH) + (10 + baseH) * easedT);
        int drawY = offsetY + baseY;
        int drawX = centerX - scaledW / 2;

        buf.fillSmoothRoundRect(drawX - 1, drawY - 1, scaledW + 2, scaledH + 2, cornerRadius, TFT_DARKGREY);
        buf.fillSmoothRoundRect(drawX, drawY, scaledW, scaledH, cornerRadius, bgColor);

        if (getTextFunc) {
            String text = getTextFunc();
            buf.setTextColor(TFT_BLACK);
            buf.setTextDatum(TL_DATUM);
            buf.setTextSize(2);

            int textX = drawX + (int)(10 * scale);
            int textY = drawY + (int)(10 * scale);
            int maxWidth = scaledW - (int)(20 * scale);
            int lineHeight = (int)((buf.fontHeight() + 4) * scale);
            int cursorY = textY;

            int start = 0, len = text.length();
            while (start < len && (cursorY + lineHeight) <= (drawY + scaledH - (int)(10 * scale))) {
                int end = start, lastFit = start;
                while (end < len) {
                    String line = text.substring(start, end + 1);
                    if (buf.textWidth(line) * scale > maxWidth) break;
                    lastFit = end + 1;
                    end++;
                }
                if (lastFit == start) lastFit = start + 1;
                buf.drawString(text.substring(start, lastFit), textX, cursorY);
                cursorY += lineHeight;
                start = lastFit;
            }
        }
    }

    void handleTouch(int tx, int ty) override {
        if (animProgress <= 0.0f) return;
        if (tx < 0 || ty < 0) return;

        if (onClickCallback && containsPoint(tx, ty)) {
            onClickCallback();
        }

        if (!animHiding) {
            animHiding = true;
            animShowing = false;
            animStartTime = millis();
        }
    }

    bool containsPoint(int px, int py) {
        if (animProgress <= 0.0f) return false;
        float easedT = easeOutQuad(animProgress);
        float scale = 0.7f + 0.3f * easedT;

        int baseW = width;
        int baseH = popupHeight;
        int scaledW = (int)(baseW * scale);
        int scaledH = (int)(baseH * scale);

        int centerX = x + bufWidth() / 2;
        int baseY = (int)((-baseH) + (10 + baseH) * easedT);
        int drawY = y + baseY;
        int drawX = centerX - scaledW / 2;

        return px >= drawX && px <= (drawX + scaledW) &&
               py >= drawY && py <= (drawY + scaledH);
    }

    int bufWidth() const { return width + 20; }
    int bufHeight() const { return popupHeight + 100; }
};
