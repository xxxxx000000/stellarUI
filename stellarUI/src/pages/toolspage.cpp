#include "ToolsPage.h"
#include "../UIManager.h"

extern UIManager uiManager;
extern BrightnessService brightnessservice;
extern SleepService sleepservice;
extern BatteryService batteryservice;
extern InputService inputservice;
extern IMUService imuservice;

ToolsPage::ToolsPage() {
    auto* background = new ColorBlock(120, 140, 240, 280, TFT_DARKGREY, 0);
    addChild(background);
    auto* btn1 = new Button(60, 80, 100, 100, TFT_WHITE, ButtonShape::RoundedRect, 20, 0x5DED);
    btn1->onClickCallback = [this]() {
        // uiManager.pushPage(new NotePage(), TransitionType::SlideLeft);
    };
    addChild(btn1);
    auto* txt1 = new Text(60, 80, "BLE", 2, TFT_BLACK, true);
    addChild(txt1);
    auto* btn2 = new Button(180, 80, 100, 100, TFT_WHITE, ButtonShape::RoundedRect, 20, 0xDFFF);
    btn2->onClickCallback = [this]() {
        // uiManager.pushPage(new QRScannerPage(), TransitionType::SlideLeft);
    };
    addChild(btn2);
    auto* txt2 = new Text(180, 80, "WiFi", 2, TFT_BLACK, true);
    addChild(txt2);
    auto* btn3 = new Button(60, 200, 100, 100, TFT_WHITE, ButtonShape::RoundedRect, 20, 0xBDF7);
    btn3->onClickCallback = [this]() {
        // uiManager.pushPage(new CalculatorPage(), TransitionType::SlideLeft);
    };
    addChild(btn3);
    auto* txt3 = new Text(60, 200, "Calc", 2, TFT_BLACK, true);
    addChild(txt3);
    auto* btn4 = new Button(180, 200, 100, 100, TFT_WHITE, ButtonShape::RoundedRect, 20, 0xCFFF);
    btn4->onClickCallback = [this]() {
        uiManager.pushPage(new MainPage(), TransitionType::SlideUp);
    };
    addChild(btn4);
    auto* txt4 = new Text(180, 200, "Back", 2, TFT_BLACK, true);
    addChild(txt4);
}
