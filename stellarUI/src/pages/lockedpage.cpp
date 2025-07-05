#include "LockedPage.h"
#include "MainPage.h"  
#include "../UIManager.h" 

extern UIManager uiManager;
extern RTCService rtcservice;
extern FrameRateService framerateservice;

LockedPage::LockedPage() {
    auto* frameText = new LiveText(200, 20, TFT_WHITE, []() {
        return String(framerateservice.getFPS());
    },2);
    addChild(frameText);
    auto* background = new StarryBackground(240, 280, 80);
    addChild(background);
    auto* label = new Text(120, 260, "^", 3, TFT_WHITE, true);
    addChild(label);
    auto* gesture = new GestureDetector(120,140,240,280,30);
    auto* clock = new ClockText(120, 130, 4, TFT_WHITE, true);
    addChild(clock);
    gesture->onSwipeUp = []() {
        uiManager.pushPage(new MainPage(),TransitionType::SlideUp);
    };
    addChild(gesture);

}