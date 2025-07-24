#include "wifipage.h"
#include "../UIManager.h"

extern UIManager uiManager;
extern ScreenRotationService screenrotationservice;

uint8_t rt = 1;

WifiPage::WifiPage() {
    auto* background = new ColorBlock(0.5f, 0.5f, 1.0f, 1.0f, 0x1020, 30);
    addChild(background);
    auto* backBtn = new Button(0.125f, 0.107f, 0.333f, 0.143f, 0x4090, ButtonShape::Circle, 30);
    addChild(backBtn);
    backBtn->onClickCallback = [this]() {
        uiManager.pushPage(new ToolsPage(), TransitionType::SlideRight);
    };
    addChild(new Text(0.125f, 0.107f, "<", 2, 0xFFFF, true));

    auto* rBtn = new Button(0.5f, 0.5f, 0.333f, 0.333f, 0x4090, ButtonShape::Circle, 30);
    addChild(rBtn);
    rBtn->onClickCallback = [this]() {
        screenrotationservice.setRotation(rt);
        rt++;
        if(rt == 4){
            rt = 0;
        }
    };
    auto* icon = new IconComponent(
        0.5f, 0.5f,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int r = s * 10;
            buf.drawSmoothArc(cx,cy,r,r - 1,0,270,TFT_WHITE,0x4090);
            buf.fillTriangle(cx + 8,cy,cx + 12,cy,cx + 10,cy + 5,TFT_WHITE);
        }
    );
    addChild(icon);
}
