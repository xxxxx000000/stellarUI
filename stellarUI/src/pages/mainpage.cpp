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
        uiManager.pushPage(new setTimePage(),TransitionType::SlideDown);
    };
    addChild(btn2);
    auto* btn3 = new Button(150,215,110,40,TFT_WHITE,ButtonShape::RoundedRect,20,TFT_LIGHTGREY);
    btn3->onClickCallback = [this](){
        uiManager.pushPage(new PosturePage(),TransitionType::SlideLeft);
        sleepservice.setPreventSleep(true);
    };
    addChild(btn3);
    auto* txt = new Text(150,75,"lock",2,TFT_BLACK,true);
    addChild(txt);
    auto* txt2 = new Text(150,145,"set-time",2,TFT_BLACK,true);
    addChild(txt2);
    auto* txt3 = new Text(150,215,"posture",2,TFT_BLACK,true);
    addChild(txt3);


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
    });
    addChild(popup);
}