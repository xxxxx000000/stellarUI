#include "posturepage.h"
#include "../UIManager.h"

extern UIManager uiManager;
#ifdef imuService
extern IMUService imuservice;
#endif
#ifdef sleepService
extern SleepService sleepservice;
#endif

PosturePage::PosturePage() {
    auto* background = new ColorBlock(0.5f, 0.5f, 1.0f, 1.0f, 0x1082, 30);
    addChild(background);
    auto* backBtn = new Button(0.125f, 0.107f, 0.333f,0.143f, 0xD75F, ButtonShape::Circle, 30);
    addChild(backBtn);
    backBtn->onClickCallback = [this]() {
        uiManager.pushPage(new MainPage(), TransitionType::UncoverRight);
        #ifdef sleepService
        sleepservice.setPreventSleep(false);
        #endif
    };
    addChild(new Text(0.125f, 0.107f, "<", 2, 0xFFFF, true));

    liveText = new LiveText(0.5f, 0.107f, 0xFFFF, []() {
        #ifdef imuService
        return String(imuservice.getLinearAccelerationMagnitude());
        #endif
        return String("---");
    }, 2);
    addChild(liveText);

    linechart = new LineChartComponent(0.083f, 0.25f, 0.833f, 0.357f, []() {
        #ifdef imuService
        return imuservice.getLinearAccelerationMagnitude();
        #endif
        return 0.0f;
    }, 20);
    addChild(linechart);

    auto* stopBtn = new Button(0.5f, 0.714f, 0.333f, 0.107f, 0x001F, ButtonShape::RoundedRect);
    addChild(stopBtn);
    auto* stopTxt = new Text(0.5f, 0.714f, "Stop Sampling", 1, 0xFFFF, true);
    addChild(stopTxt);

    stopBtn->onClickCallback = [this]() {
        linechart->pauseSampling();
    };
    auto* zoomOutBtn = new Button(0.125f, 0.714f, 0.208f, 0.107f, 0x8410, ButtonShape::RoundedRect);
    addChild(zoomOutBtn);
    addChild(new Text(0.125f, 0.714f, "-", 2, 0xFFFF, true));
    zoomOutBtn->onClickCallback = [this]() {
        linechart->zoomOut();
    };
    auto* zoomInBtn = new Button(0.875f, 0.714f, 0.208f, 0.107f, 0x8410, ButtonShape::RoundedRect);
    addChild(zoomInBtn);
    addChild(new Text(0.875f, 0.714f, "+", 2, 0xFFFF, true));
    zoomInBtn->onClickCallback = [this]() {
        linechart->zoomIn();
    };
    auto* swch = new Switch(0.5f, 0.893f, 0.417f, 20, true);
    swch->onClickCallback = [this](bool state) {
        liveText->enabled = state;
    };
    addChild(swch);
}
