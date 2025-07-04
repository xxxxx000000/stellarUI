#pragma once
#include "../UIComponents/page.h"
#include "../UIComponents/text.h"
#include "../UIComponents/button.h"
#include "../Services/RTCService.h"
#include "LockedPage.h"
#include "../UIManager.h"

class setTimePage : public Page {
private:
    enum class Step {
        Year, Month, Day, Hour, Minute, Second, Done
    };

    Step currentStep;
    int year, month, day, hour, minute, second;

    Text* displayText;
    Text* prevLabel;
    Text* nextLabel;
    Text* incLabel;
    Text* decLabel;
    Text* skipLabel;

    void skipTimeSetting();
    void updateText();
    void nextStep();
    void prevStep();
    void increase();
    void decrease();

public:
    setTimePage();
};
