#pragma once
#include <TFT_eSPI.h>
#include "component.hpp"

/*
    图像组件，支持16位色深的图像数据，绘制在指定位置，性能消耗大，不建议使用
    Image Component that supports 16-bit color depth image data and drawing at a specific location. Not recommended for possible bad performance
*/

class ImageComponent : public Component {
private:
    const uint16_t* imageData;
    int imgWidth, imgHeight;
    int screenWidth,screenHeight;
    bool flexLayout = false;
public:
    ImageComponent(int _x, int _y, const uint16_t* _imageData, int _imgWidth, int _imgHeight)
        : imageData(_imageData), imgWidth(_imgWidth), imgHeight(_imgHeight) {
        x = _x;
        y = _y;
        targetX = x;
        targetY = y;
        targetScale = scale = 1.0f;
        enabled = true;
    }
    ImageComponent(float _fx, float _fy, const uint16_t* _imageData, int _imgWidth, int _imgHeight)
        : Component(_fx,_fy), imageData(_imageData), imgWidth(_imgWidth), imgHeight(_imgHeight) {
        targetX = x;
        targetY = y;
        targetScale = scale = 1.0f;
        enabled = true;
        flexLayout = true;
    }

    virtual void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;
        screenWidth = buf.width();
        screenHeight = buf.height();

        int drawX;
        int drawY;
        if(!flexLayout){
            drawX = x + offsetX - imgWidth / 2;
            drawY = y + offsetY - imgHeight / 2;
        }else{
            drawX = fx * screenWidth + offsetX - imgWidth / 2;
            drawY = fy * screenHeight + offsetY - imgHeight / 2;
        }

        buf.pushImage(drawX, drawY, imgWidth, imgHeight, imageData);
    }

    virtual void update(uint32_t now) override {
        Component::update(now);
    }

    virtual void onClick() override {
        Component::onClick();
    }

    int getImageWidth() const {
        return imgWidth;
    }

    int getImageHeight() const {
        return imgHeight;
    }

    virtual bool containsPoint(int px, int py) override {
        int drawX = x - imgWidth / 2;
        int drawY = y - imgHeight / 2;

        return (px >= drawX && px <= drawX + imgWidth) && (py >= drawY && py <= drawY + imgHeight);
    }
};
