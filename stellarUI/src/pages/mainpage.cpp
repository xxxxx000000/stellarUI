#include "MainPage.h"
#include "../UIManager.h"

extern UIManager uiManager;
extern BrightnessService brightnessservice;
extern SleepService sleepservice;
extern BatteryService batteryservice;
extern InputService inputservice;
extern IMUService imuservice;

MainPage::MainPage() {
    auto* background = new StarryBackground(240, 280, 80);
    addChild(background);

    auto* batterytext = new LiveText(200, 20, TFT_WHITE, []() {
        return String((int)batteryservice.getPercentage()) + "%";
    },2);
    addChild(batterytext);
    auto* clock = new ClockText(70, 20, 2, TFT_WHITE, true);
    addChild(clock);
    auto* block = new ColorBlock(120,150,200,220,TFT_DARKGREY,30);
    addChild(block);
    auto* slider = new Slider(40, 55, 40, 180, 20, SliderDirection::Vertical, TFT_BLACK, TFT_WHITE, 0, brightnessservice.getBrightness());
    addChild(slider);
    slider->onChange = [](uint8_t v) {
        brightnessservice.setBrightness(v);
    };
    auto* btn = new Button(150,75,110,40,TFT_WHITE,ButtonShape::RoundedRect,20,TFT_LIGHTGREY);
    btn->onClickCallback = [this](){
        uiManager.pushPage(new LockedPage(),TransitionType::SlideDown);
    };
    btn->customEventName = "click";
    inputservice.registerInput(12, btn, 0, "click");
    addChild(btn);
    auto* btn2 = new Button(150,145,110,40,TFT_WHITE,ButtonShape::RoundedRect,20,TFT_LIGHTGREY);
    btn2->onClickCallback = [this](){
        uiManager.pushPage(new PosturePage(),TransitionType::SlideLeft);
        sleepservice.setPreventSleep(true);
    };
    addChild(btn2);
    auto* btn3 = new Button(150,215,110,40,TFT_WHITE,ButtonShape::RoundedRect,20,TFT_LIGHTGREY);
    btn3->onClickCallback = [this](){
       uiManager.pushPage(new ToolsPage(),TransitionType::SlideLeft);
    };
    addChild(btn3);
    auto* txt = new Text(160,75,"lock",2,TFT_BLACK,true);
    addChild(txt);
    auto* txt2 = new Text(160,145,"imu",2,TFT_BLACK,true);
    addChild(txt2);
    auto* txt3 = new Text(160,215,"more",2,TFT_BLACK,true);
    addChild(txt3);

    
    auto* icon = new IconComponent(
        120, 75,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int w = 16 * s, h = 20 * s;
            int r = 5 * s;
            buf.fillSmoothRoundRect(cx - w/2, cy - h/2 + r, w, h - r - 2, 3 * s, TFT_BLACK);
            buf.drawSmoothArc(cx, cy - h/2 + r, r,r - 2, 90, 270, TFT_BLACK,TFT_WHITE);
        }
    );
    addChild(icon);

    auto* icon2 = new IconComponent(
        120, 145,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int len = 7 * s;
            buf.drawWideLine(cx, cy, cx + len, cy - len, 2, TFT_RED);
            buf.drawWideLine(cx, cy, cx - len, cy - len, 2, TFT_GREEN);
            buf.drawWideLine(cx, cy, cx, cy + len, 2, TFT_BLUE);
        }
    );
    addChild(icon2);
    auto* icon3 = new IconComponent(
        120, 215,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int dotSize = 4 * s;
            int gap = 4 * s;
            uint16_t colors[4] = {
                buf.color565(0, 0, 255),
                buf.color565(0, 100, 255),
                buf.color565(0, 150, 255),
                buf.color565(100, 200, 255)
            };
            int i = 0;
            for (int dx = -1; dx <= 1; dx += 2) {
                for (int dy = -1; dy <= 1; dy += 2) {
                    int px = cx + dx * gap - dotSize / 2;
                    int py = cy + dy * gap - dotSize / 2;
                    buf.fillRect(px, py, dotSize, dotSize, colors[i++]);
                }
            }
        }
    );
    addChild(icon3);



    auto popupTextLambda = []() -> String {
        return "acceleration > 0.5";
    };
    auto popupShowLambda = []() -> bool {
        return imuservice.getLinearAccelerationMagnitude() > 0.5f; 
    };
    auto* popup = new PopupComponent(
        240 - 20,
        60,
        popupShowLambda,
        popupTextLambda,
        TFT_WHITE,
        30
    );
    popup->setOnClick([]() {
        uiManager.pushPage(new PosturePage(),TransitionType::SlideDown);
        sleepservice.setPreventSleep(true);
    });
    addChild(popup);
}