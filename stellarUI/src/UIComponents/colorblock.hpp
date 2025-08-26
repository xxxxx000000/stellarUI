#pragma once
#include <TFT_eSPI.h>
#include "component.hpp"

/*
    色块组件，装饰作用，参数为横纵坐标，长宽，颜色和圆角半径
    Color block，used for decoration, with parameters: X and Y coordinates, width, height, color, and corner radius
*/

class ColorBlock : public Component {
public:
    int width = 50;
    int height = 30;
    int radius = 8;

    float fw, fh;
    bool flexLayout = false;

    int screenWidth;
    int screenHeight;

    ColorBlock(int _x, int _y, int _w, int _h, uint16_t _color, int _radius = 8)
        : Component(_x, _y, _color), width(_w), height(_h), radius(_radius), flexLayout(false) {
        targetX = x;
        targetY = y;
        targetScale = scale = 1.0f;
        enabled = true;
    }

    ColorBlock(float _fx, float _fy, float _fw, float _fh, uint16_t _color, int _radius = 8)
        : Component(_fx, _fy, _color), fw(_fw), fh(_fh), radius(_radius), flexLayout(true) {
        targetScale = scale = 1.0f;
        enabled = true;
    }

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        screenWidth = buf.width();
        screenHeight = buf.height();

        int scaledW, scaledH;
        int drawX, drawY;

        if (!flexLayout) {
            scaledW = (int)(width * scale);
            scaledH = (int)(height * scale);
            drawX = x + offsetX - scaledW / 2;
            drawY = y + offsetY - scaledH / 2;
        } else {
            scaledW = (int)(fw * screenWidth * scale);
            scaledH = (int)(fh * screenHeight * scale);
            drawX = (int)(fx * screenWidth) + offsetX - scaledW / 2;
            drawY = (int)(fy * screenHeight) + offsetY - scaledH / 2;
        }

        buf.fillSmoothRoundRect(drawX, drawY, scaledW, scaledH, radius, color);
    }

    virtual void update(uint32_t now) override {
        Component::update(now);
    }

    virtual bool containsPoint(int px, int py) override {
        if (!enabled) return false;

        int centerX, centerY;
        int halfW, halfH;

        if (!flexLayout) {
            centerX = x;
            centerY = y;
            halfW = (int)(width * scale) / 2;
            halfH = (int)(height * scale) / 2;
        } else {
            centerX = (int)(fx * screenWidth);
            centerY = (int)(fy * screenHeight);
            halfW = (int)(fw * screenWidth * scale) / 2;
            halfH = (int)(fh * screenHeight * scale) / 2;
        }

        return (px >= centerX - halfW) && (px <= centerX + halfW) &&
               (py >= centerY - halfH) && (py <= centerY + halfH);
    }
};
