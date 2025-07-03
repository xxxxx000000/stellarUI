#include "settimepage.h"

extern UIManager uiManager;
extern RTCService rtcservice;

setTimePage::setTimePage()
    : currentStep(Step::Year), year(2025), month(1), day(1), hour(12), minute(30), second(0)
{
    auto* displayText = new Text(120, 60, "", 2, TFT_WHITE, true);
    addChild(displayText);

    auto* incBtn = new Button(210, 60, 40, 40, TFT_GREEN, ButtonShape::RoundedRect);
    incBtn->onClickCallback = [this]() { increase(); };
    addChild(incBtn);
    auto* incLabel = new Text(210, 60, "+", 2, TFT_WHITE, true);
    addChild(incLabel);

    auto* decBtn = new Button(30, 60, 40, 40, TFT_RED, ButtonShape::RoundedRect);
    decBtn->onClickCallback = [this]() { decrease(); };
    addChild(decBtn);
    auto* decLabel = new Text(30, 60, "-", 2, TFT_WHITE, true);
    addChild(decLabel);

    auto* prevBtn = new Button(60, 200, 80, 40, TFT_BLUE, ButtonShape::RoundedRect);
    prevBtn->onClickCallback = [this]() { prevStep(); };
    addChild(prevBtn);
    auto* prevLabel = new Text(60, 200, "Prev", 2, TFT_WHITE, true);
    addChild(prevLabel);

    auto* nextBtn = new Button(180, 200, 80, 40, TFT_BLUE, ButtonShape::RoundedRect);
    nextBtn->onClickCallback = [this]() { nextStep(); };
    addChild(nextBtn);
    auto* nextLabel = new Text(180, 200, "Next", 2, TFT_WHITE, true);
    addChild(nextLabel);
    auto* skipBtn = new Button(120, 130, 80, 40, TFT_DARKGREY, ButtonShape::RoundedRect);
    skipBtn->onClickCallback = [this]() { skipTimeSetting(); };
    addChild(skipBtn);
    auto* skipLabel = new Text(120, 130, "done", 2, TFT_WHITE, true);
    addChild(skipLabel);

    updateText();
}

void setTimePage::skipTimeSetting() {
    struct tm now;
    rtcservice.getTime(now);

    year = now.tm_year;
    month = now.tm_mon;
    day = now.tm_mday;
    hour = now.tm_hour;
    minute = now.tm_min;
    second = now.tm_sec;

    uiManager.pushPage(new LockedPage(), TransitionType::SlideUp);
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
        uiManager.pushPage(new LockedPage(), TransitionType::SlideUp);
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
