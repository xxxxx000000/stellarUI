#include "wifipage.h"
#include "../UIManager.h"

extern UIManager uiManager;

WifiPage::WifiPage() {
    auto* background = new ColorBlock(120, 140, 240, 280, TFT_BLACK, 30);
    addChild(background);

    auto* backBtn = new Button(30, 30, 80, 40, TFT_LIGHTGREY, ButtonShape::Circle,30);
    addChild(backBtn);
    backBtn->onClickCallback = [this]() {
        uiManager.pushPage(new ToolsPage(), TransitionType::SlideRight);
    };
    addChild(new Text(30, 30, "<", 2, TFT_WHITE, true));

    std::vector<String> paragraphs = {
        "This is the first paragraph. It might be a bit long and will wrap automatically.",
        "Second paragraph is here, also auto-wrapped if too long.",
        "Third paragraph just to demonstrate multiple segments."
    };
    MultilineText* msg = new MultilineText(120, 150, 200, 200, 2, TFT_WHITE);
    msg->setTextLines(paragraphs);
    addChild(msg);

}