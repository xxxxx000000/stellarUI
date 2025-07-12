#pragma once
#include "button.h"

/*
    透明按钮组件，支持透明度设置，性能消耗大不建议使用
    Transparent button component that supports setting transparency, not recommended for performance reasons
*/

class TransparentButton : public Button {
private:
    float alpha;

public:
    TransparentButton(int _x, int _y, int _w, int _h, uint16_t _color,
                  ButtonShape _shape = ButtonShape::RoundedRect,
                  int _radius = 8, uint16_t _pressedColor = TFT_DARKGREY,
                  float _alpha = 0.5f)
        : Button(_x, _y, _w, _h, _color, _shape, _radius, _pressedColor),
          alpha(_alpha) {}

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        int scaledW = width * scale;
        int scaledH = height * scale;

        int drawX = x + offsetX - scaledW / 2;
        int drawY = y + offsetY - scaledH / 2;

        uint16_t drawColor = pressed ? pressedColor : color;

        uint16_t* backgroundBuffer = new uint16_t[scaledW * scaledH];

        for (int i = 0; i < scaledW; i++) {
            for (int j = 0; j < scaledH; j++) {
                backgroundBuffer[j * scaledW + i] = buf.readPixel(drawX + i, drawY + j);
            }
        }

        switch (shape) {
            case ButtonShape::Rectangle:
                buf.fillRect(drawX, drawY, scaledW, scaledH, drawColor);
                break;
            case ButtonShape::RoundedRect:
                buf.fillSmoothRoundRect(drawX, drawY, scaledW, scaledH, radius, drawColor);
                break;
            case ButtonShape::Circle: {
                int r = (scaledW < scaledH ? scaledW : scaledH) / 2;
                buf.fillSmoothCircle(x + offsetX, y + offsetY, r, drawColor);
                break;
            }
        }

        if (alpha < 1.0f) {
            applyTransparency(buf, backgroundBuffer, drawX, drawY, scaledW, scaledH, alpha);
        }

        delete[] backgroundBuffer;
    }

    void applyTransparency(TFT_eSprite& buf, uint16_t* backgroundBuffer, int drawX, int drawY, 
                           int scaledW, int scaledH, float alpha) {
        for (int i = drawX; i < drawX + scaledW; i++) {
            for (int j = drawY; j < drawY + scaledH; j++) {
                uint16_t color = buf.readPixel(i, j);
                uint16_t bgColor = backgroundBuffer[(j - drawY) * scaledW + (i - drawX)];
                
                uint16_t blendedColor = blendWithAlpha(color, bgColor, alpha);
                buf.drawPixel(i, j, blendedColor);
            }
        }
    }

    uint16_t blendWithAlpha(uint16_t color, uint16_t bgColor, float alpha) {
        uint8_t r1 = (color >> 11) & 0x1F;
        uint8_t g1 = (color >> 5) & 0x3F;
        uint8_t b1 = color & 0x1F;

        uint8_t r2 = (bgColor >> 11) & 0x1F;
        uint8_t g2 = (bgColor >> 5) & 0x3F;
        uint8_t b2 = bgColor & 0x1F;

        uint8_t r = r1 * alpha + r2 * (1 - alpha);
        uint8_t g = g1 * alpha + g2 * (1 - alpha);
        uint8_t b = b1 * alpha + b2 * (1 - alpha);

        return (r << 11) | (g << 5) | b;
    }

    virtual void update(uint32_t now) override {
        Button::update(now);
    }

    virtual void onClick() override {
        Button::onClick();
    }
};
