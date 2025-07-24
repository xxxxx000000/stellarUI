#include "ToolsPage.h"
#include "../UIManager.h"

extern UIManager uiManager;
extern BrightnessService brightnessservice;
extern SleepService sleepservice;
extern BatteryService batteryservice;
extern InputService inputservice;
extern IMUService imuservice;

ToolsPage::ToolsPage() {
    auto* background = new ColorBlock(0.5f, 0.5f, 1.0f, 1.0f, 0x1082, 30);
    addChild(background);
    auto* btn1 = new Button(0.250f, 0.286f, 0.417f, 0.357f, 0xFFFF, ButtonShape::RoundedRect, 20, 0x5DED);
    btn1->onClickCallback = [this]() {
        uiManager.pushPage(new BluetoothPage(), TransitionType::SlideLeft);
    };
    addChild(btn1);
    auto* txt1 = new Text(0.25f, 0.214f, "BLE", 2, 0x0000, true);
    addChild(txt1);
    auto* btn2 = new Button(0.750f, 0.286f, 0.417f, 0.357f, 0xFFFF, ButtonShape::RoundedRect, 20, 0xDFFF);
    btn2->onClickCallback = [this]() {
        uiManager.pushPage(new WifiPage(), TransitionType::SlideLeft);
    };
    addChild(btn2);
    auto* txt2 = new Text(0.75f, 0.214f, "WiFi", 2, 0x0000, true);
    addChild(txt2);
    auto* btn3 = new Button(0.250f, 0.714f, 0.417f, 0.357f, 0xFFFF, ButtonShape::RoundedRect, 20, 0xBDF7);
    btn3->onClickCallback = [this]() {
        uiManager.pushPage(new setTimePage(), TransitionType::SlideLeft);
    };
    addChild(btn3);
    auto* txt3 = new Text(0.25f, 0.643f, "Time", 2, 0x0000, true);
    addChild(txt3);
    auto* btn4 = new Button(0.750f, 0.714f, 0.417f, 0.357f, 0xFFFF, ButtonShape::RoundedRect, 20, 0xCFFF);
    btn4->onClickCallback = [this]() {
        uiManager.pushPage(new MainPage(), TransitionType::SlideRight);
    };
    addChild(btn4);
    auto* txt4 = new Text(0.75f, 0.714f, "Back", 2, 0x0000, true);
    addChild(txt4);
    auto* icon = new IconComponent(
        0.233f, 0.321f,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int h = 20 * s;
            int w = 8 * s;
            int thickness = 2;
            buf.drawWideLine(cx, cy - h / 2, cx, cy + h / 2, 2, 0x001F);
            buf.drawWideLine(cx - 4, cy + 4, cx + w, cy - h / 4, 2, 0x001F);
            buf.drawWideLine(cx, cy - h / 2, cx + w, cy - h / 4, 2, 0x001F);
            buf.drawWideLine(cx - 4, cy - 4, cx + w, cy + h / 4, 2, 0x001F);
            buf.drawWideLine(cx, cy + h / 2, cx + w, cy + h / 4, 2, 0x001F);
        }
    );
    addChild(icon);
    auto* icon2 = new IconComponent(
        0.75f, 0.357f,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int r1 = 4 * s;
            int r2 = 10 * s;
            int r3 = 16 * s;

            buf.drawSmoothArc(cx, cy, r3, r3 + 2, 140, 220, 0xFD20, 0xFFFF);
            buf.drawSmoothArc(cx, cy, r2, r2 + 2, 150, 210, 0xFD20, 0xFFFF);
            buf.drawSmoothArc(cx, cy, r1, r1 + 2, 160, 200, 0xFD20, 0xFFFF);

            buf.fillSmoothCircle(cx, cy, 1 * s, 0xFD20);
        }
    );
    addChild(icon2);
    auto* icon3 = new IconComponent(
        0.25f, 0.75f,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int r = 14 * s;
            buf.drawSmoothCircle(cx, cy, r, 0x0000, 0xFFFF);
            buf.drawWideLine(cx, cy, cx, cy - r / 2, 2, 0x0000); 
            buf.drawWideLine(cx, cy, cx + r / 2, cy, 2, 0xF800);
            buf.fillSmoothCircle(cx, cy, 2 * s, 0x0000);
        }
    );
    addChild(icon3);
}
