#include "LockedPage.hpp"
#include "MainPage.hpp"  
#include "../UIManager.hpp" 

extern UIManager uiManager;
#ifdef framerateService
extern FrameRateService framerateservice;
#endif

LockedPage::LockedPage() {
    auto* background = new StarryBackground(240,280,120);
    addChild(background);
    auto* frameText = new LiveText(0.833f, 0.071f, 0xDEFB, []() {
        #ifdef framerateService
        return String(framerateservice.getFPS());
        #endif
        return String("--");
    },2);
    addChild(frameText);
    auto* label = new Text(0.5f, 0.929f, "^", 3, 0xDEFB, true);
    addChild(label);
    auto* gesture = new GestureDetector(0.5f,0.85f,1.0f,0.3f,30);
    auto* clock = new ClockText(0.5f, 0.464f, 4, TFT_WHITE, true);
    addChild(clock);
    gesture->onSwipeUp = []() {
        uiManager.pushPage(new MainPage(),TransitionType::SlideUp);
    };
    addChild(gesture);

    Animator* anim = new Animator();
    anim->addFloatAnim(&label->fy, 0.929f, 0.893f, 150, EasingType::EaseOutQuad, 0);
    anim->addFloatAnim(&label->fy, 0.893f, 0.929f, 150, EasingType::EaseOutQuad, 150);
    anim->addFloatAnim(&label->fy, 0.929f, 0.893f, 150, EasingType::EaseOutQuad, 1650);
    anim->addFloatAnim(&label->fy, 0.893f, 0.929f, 150, EasingType::EaseOutQuad, 1800);
    anim->setLoop(true);
    anim->start();
    addAnimator(anim);

}
