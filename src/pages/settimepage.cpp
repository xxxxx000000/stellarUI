#include "settimepage.hpp"

extern UIManager uiManager;
#ifdef rtcService
extern RTCService rtcservice;
#endif

setTimePage::setTimePage()
    : currentStep(Step::Year), year(2025), month(1), day(1), hour(12), minute(30), second(0)
{
    #ifdef rtcService
    auto* background = new ColorBlock(0.5f, 0.5f, 1.0f, 1.0f, 0x1082, 30);
    addChild(background);

    displayText = new Text(0.5f, 0.214f, "", 2, 0xFFFF, true);
    addChild(displayText);

    auto* incBtn  = new Button(0.875f, 0.214f, 0.167f, 0.143f, 0x07E0, ButtonShape::RoundedRect);
    incBtn->onClickCallback = [this]() { increase(); };
    addChild(incBtn);
    auto* incLabel = new Text(0.875f, 0.214f, "+", 2, 0xFFFF, true);
    addChild(incLabel);
    
    auto* decBtn  = new Button(0.125f, 0.214f, 0.167f, 0.143f, 0xF800, ButtonShape::RoundedRect);
    decBtn->onClickCallback = [this]() { decrease(); };
    addChild(decBtn);
    auto* decLabel = new Text(0.125f, 0.214f, "-", 2, 0xFFFF, true);
    addChild(decLabel);

    auto* prevBtn = new Button(0.250f, 0.714f, 0.333f, 0.143f, 0x001F, ButtonShape::RoundedRect);
    prevBtn->onClickCallback = [this]() { prevStep(); };
    addChild(prevBtn);
    auto* prevLabel = new Text(0.250f, 0.714f, "Prev", 2, 0xFFFF, true);
    addChild(prevLabel);

    auto* nextBtn = new Button(0.750f, 0.714f, 0.333f, 0.143f, 0x001F, ButtonShape::RoundedRect);
    nextBtn->onClickCallback = [this]() { nextStep(); };
    addChild(nextBtn);
    nextLabel = new Text(0.750f, 0.714f, "Next", 2, 0xFFFF, true);
    addChild(nextLabel);
    
    auto* skipBtn = new Button(0.500f, 0.464f, 0.333f, 0.143f, 0x8410, ButtonShape::RoundedRect);
    skipBtn->onClickCallback = [this]() { skipTimeSetting(); };
    addChild(skipBtn);
    auto* skipLabel = new Text(0.500f, 0.464f, "done", 2, 0xFFFF, true);
    addChild(skipLabel);
    
    updateText();
    #else
    auto* backBtn = new Button(0.125f, 0.107f, 0.333f, 0.143f, 0x4090, ButtonShape::Circle, 30);
    addChild(backBtn);
    backBtn->onClickCallback = [this]() {
        uiManager.pushPage(new ToolsPage(), TransitionType::UncoverRight);
    };
    auto* errorText = new Text(0.5f, 0.5f, "RTC Service Not Enabled", 2, 0xFFFF, true);
    addChild(errorText);
    #endif
}

#ifdef rtcService
void setTimePage::skipTimeSetting() {
    struct tm now;
    rtcservice.getTime(now);

    year = now.tm_year;
    month = now.tm_mon;
    day = now.tm_mday;
    hour = now.tm_hour;
    minute = now.tm_min;
    second = now.tm_sec;

    uiManager.pushPage(new ToolsPage(), TransitionType::UncoverRight);
}

void setTimePage::updateText() {
    String text;
    switch (currentStep) {
        case Step::Year: text = "Year: " + String(year); break;
        case Step::Month: text = "Month: " + String(month); break;
        case Step::Day: text = "Day: " + String(day); break;
        case Step::Hour: text = "Hour: " + String(hour); break;
        case Step::Minute: text = "Minute: " + String(minute); break;
        case Step::Second: text = "Second: " + String(second); break;
        case Step::Done: text = "Confirming..."; break;
    }
    displayText->setText(text);

    if (currentStep == Step::Second) {
        nextLabel->setText("Confirm");
    } else {
        nextLabel->setText("Next");
    }
}

void setTimePage::nextStep() {
    if (currentStep == Step::Second) {
        rtcservice.setDateTime(year, month, day, hour, minute, second);
        uiManager.pushPage(new ToolsPage(), TransitionType::UncoverRight);
    } else {
        currentStep = static_cast<Step>(static_cast<int>(currentStep) + 1);
    }
    updateText();
}

void setTimePage::prevStep() {
    if (static_cast<int>(currentStep) > 0) {
        currentStep = static_cast<Step>(static_cast<int>(currentStep) - 1);
    }
    updateText();
}

void setTimePage::increase() {
    switch (currentStep) {
        case Step::Year: year++; break;
        case Step::Month: if (month < 12) month++; break;
        case Step::Day: if (day < 31) day++; break;
        case Step::Hour: if (hour < 23) hour++; break;
        case Step::Minute: if (minute < 59) minute++; break;
        case Step::Second: if (second < 59) second++; break;
        default: break;
    }
    updateText();
}

void setTimePage::decrease() {
    switch (currentStep) {
        case Step::Year: year--; break;
        case Step::Month: if (month > 1) month--; break;
        case Step::Day: if (day > 1) day--; break;
        case Step::Hour: if (hour > 0) hour--; break;
        case Step::Minute: if (minute > 0) minute--; break;
        case Step::Second: if (second > 0) second--; break;
        default: break;
    }
    updateText();
}
#endif