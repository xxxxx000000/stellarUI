#include "posturepage.h"
#include "../UIManager.h"

extern UIManager uiManager;
extern IMUService imuservice;
extern SleepService sleepservice;

PosturePage::PosturePage() {
    auto* background = new ColorBlock(120, 140, 240, 280, TFT_BLACK, 30);
    addChild(background);
    auto* backBtn = new Button(30, 30, 80, 40, TFT_LIGHTGREY, ButtonShape::Circle,30);
    addChild(backBtn);
    backBtn->onClickCallback = [this]() {
        uiManager.pushPage(new MainPage(), TransitionType::SlideRight);
        sleepservice.setPreventSleep(false);
    };
    addChild(new Text(30, 30, "<", 2, TFT_WHITE, true));

    liveText = new LiveText(120, 30, TFT_WHITE, []() {
        return String(imuservice.getLinearAccelerationMagnitude());
    }, 2);
    addChild(liveText);

    linechart = new LineChartComponent(20, 70, 200, 100, []() {
        return imuservice.getLinearAccelerationMagnitude();
    });
    addChild(linechart);

    auto* stopBtn = new Button(120, 200, 80, 30, TFT_BLUE, ButtonShape::RoundedRect);
    addChild(stopBtn);
    auto* stopTxt = new Text(120, 200, "Stop Sampling", 1, TFT_WHITE, true);
    addChild(stopTxt);

    stopBtn->onClickCallback = [this]() {
        linechart->pauseSampling();
    };
    auto* zoomOutBtn = new Button(30, 200, 50, 30, TFT_DARKGREY, ButtonShape::RoundedRect);
    addChild(zoomOutBtn);
    addChild(new Text(30, 200, "-", 2, TFT_WHITE, true));
    zoomOutBtn->onClickCallback = [this]() {
        linechart->zoomOut();
    };
    auto* zoomInBtn = new Button(210, 200, 50, 30, TFT_DARKGREY, ButtonShape::RoundedRect);
    addChild(zoomInBtn);
    addChild(new Text(210, 200, "+", 2, TFT_WHITE, true));
    zoomInBtn->onClickCallback = [this]() {
        linechart->zoomIn();
    };
    auto* swch = new Switch(120, 250,100,20,true);
    swch->onClickCallback = [this](bool state) {
        liveText->enabled = state;
    };
    addChild(swch);
}
