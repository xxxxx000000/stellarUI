#include "wifipage.h"
#include "../UIManager.h"

extern UIManager uiManager;

BluetoothPage::BluetoothPage() {
    auto* background = new ColorBlock(0.5f, 0.5f, 1.0f, 1.0f, 0x1020, 30);
    addChild(background);

    auto* backBtn = new Button(0.125f, 0.107f, 0.333f, 0.143f, 0x4090, ButtonShape::Circle, 30);
    addChild(backBtn);
    backBtn->onClickCallback = [this]() {
        uiManager.pushPage(new ToolsPage(), TransitionType::UncoverRight);
    };
    addChild(new Text(0.125f, 0.107f, "<", 2, 0xFFFF, true));

    std::vector<String> paragraphs = {
        "This is the first paragraph. It might be a bit long and will wrap automatically.",
        "Second paragraph is here, also auto-wrapped if too long.",
        "Third paragraph just to demonstrate multiple segments."
    };

    MultilineText* msg = new MultilineText(0.5f, 0.536f, 0.833f, 0.714f, 2, 0xDEF7);
    msg->setTextLines(paragraphs);
    addChild(msg);
}
