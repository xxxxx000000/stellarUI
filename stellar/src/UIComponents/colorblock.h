#pragma once
#include <TFT_eSPI.h>
#include "component.h"

/*
    色块组件，装饰作用，参数为横纵坐标，长宽，颜色和圆角半径
    Color block，used for decoration, with parameters: X and Y coordinates, width, height, color, and corner radius

*/

class ColorBlock : public Component {
public:
    int width = 50;
    int height = 30;
    int radius = 8;

    ColorBlock(int _x, int _y, int _w, int _h, uint16_t _color, int _radius = 8) {
        x = _x; y = _y;
        width = _w; height = _h;
        color = _color;
        radius = _radius;

        targetX = x;
        targetY = y;
        targetScale = scale = 1.0f;
        enabled = true;
    }

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        int scaledW = (int)(width * scale);
        int scaledH = (int)(height * scale);

        int drawX = x + offsetX - scaledW / 2;
        int drawY = y + offsetY - scaledH / 2;

        buf.fillRoundRect(drawX, drawY, scaledW, scaledH, radius, color);
    }

    virtual void update(uint32_t now) override {
        Component::update(now);
    }
    virtual bool containsPoint(int px, int py) override {
        if (!enabled) return false;

        int halfW = (int)(width * scale) / 2;
        int halfH = (int)(height * scale) / 2;

        return (px >= x - halfW) && (px <= x + halfW) &&
               (py >= y - halfH) && (py <= y + halfH);
    }
};
