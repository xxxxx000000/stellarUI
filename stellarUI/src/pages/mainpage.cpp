#include "MainPage.h"
#include "../UIManager.h"

extern UIManager uiManager;
extern BrightnessService brightnessservice;
extern SleepService sleepservice;
extern BatteryService batteryservice;
extern InputService inputservice;
extern IMUService imuservice;
extern ScreenRotationService screenrotationservice;

MainPage::MainPage() {
    auto* background = new StarryBackground(240,280,80);
    addChild(background);

    auto* batterytext = new LiveText(0.833f, 0.071f, 0xDEFB, []() {
        return String((int)batteryservice.getPercentage()) + "%";
    },2);
    addChild(batterytext);
    auto* clock = new ClockText(70, 20, 2, 0xDEFB, true);
    addChild(clock);
    auto* block = new ColorBlock(0.5f,0.536f,0.833f,0.786f,0xD6BA,30);
    addChild(block);
    auto* slider = new Slider(0.167f, 0.196f, 0.167f, 0.643f, 20, SliderDirection::Vertical, 0x1082, 0xFFFF, 0, brightnessservice.getBrightness());
    addChild(slider);
    slider->onChange = [](uint8_t v) {
        brightnessservice.setBrightness(v);
    };
    auto* btn = new Button(0.625f,0.268f,0.458f,0.143f,0x001F,ButtonShape::RoundedRect,20,0xD75F);
    btn->onClickCallback = [this](){
        uiManager.pushPage(new LockedPage(),TransitionType::SlideDown);
    };
    btn->customEventName = "click";
    inputservice.registerInput(12, btn, 0, "click");
    addChild(btn);
    auto* btn2 = new Button(0.625f,0.518f,0.458f,0.143f,0x001F,ButtonShape::RoundedRect,20,0xD75F);
    btn2->onClickCallback = [this](){
        uiManager.pushPage(new PosturePage(),TransitionType::SlideLeft);
        sleepservice.setPreventSleep(true);
    };
    addChild(btn2);
    auto* btn3 = new Button(0.625f,0.768f,0.458f,0.143f,0x001F,ButtonShape::RoundedRect,20,0xD75F);
    btn3->onClickCallback = [this](){
       uiManager.pushPage(new ToolsPage(),TransitionType::SlideLeft);
    };
    addChild(btn3);
    auto* txt = new Text(0.688f,0.268f,"lock",2,0xFFFF,true);
    addChild(txt);
    auto* txt2 = new Text(0.688f,0.518f,"imu",2,0xFFFF,true);
    addChild(txt2);
    auto* txt3 = new Text(0.688f,0.768f,"more",2,0xFFFF,true);
    addChild(txt3);

    auto* icon = new IconComponent(
        0.483f, 0.268f,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int w = 16 * s, h = 20 * s;
            int r = 5 * s;
            int r0 = 15 * s;
            buf.fillSmoothCircle(cx,cy,r0,TFT_WHITE);
            buf.fillSmoothRoundRect(cx - w/2, cy - h/2 + r, w, h - r - 2, 3 * s, 0x1082);
            buf.drawSmoothArc(cx, cy - h/2 + r, r,r - 2, 90, 270, 0x1082,0xFFFF);
        }
    );
    addChild(icon);

    auto* icon2 = new IconComponent(
        0.483f, 0.518f,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int len = 7 * s;
            int r0 = 15 * s;
            buf.fillSmoothCircle(cx,cy,r0,TFT_WHITE);
            buf.drawWideLine(cx, cy, cx + len, cy - len, 2, 0xF800);
            buf.drawWideLine(cx, cy, cx - len, cy - len, 2, 0x07E0);
            buf.drawWideLine(cx, cy, cx, cy + len, 2, 0x001F);
        }
    );
    addChild(icon2);
    auto* icon3 = new IconComponent(
        0.483f, 0.768f,
        [](TFT_eSprite& buf, int cx, int cy, float s) {
            int dotSize = 4 * s;
            int gap = 4 * s;
            int r0 = 15 * s;
            buf.fillSmoothCircle(cx,cy,r0,TFT_WHITE);
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
        0.215f,
        popupShowLambda,
        popupTextLambda,
        0xFFFF,
        30
    );
    popup->setOnShow([](){

    });
    popup->setOnClick([]() {
        uiManager.pushPage(new PosturePage(),TransitionType::SlideDown);
        sleepservice.setPreventSleep(true);
    });
    addChild(popup);
}
