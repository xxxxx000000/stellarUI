#pragma once
#include "../UIComponents/page.hpp"
#include "../UIComponents/colorblock.hpp"
#include "../UIComponents/text.hpp"
#include "../UIComponents/button.hpp"
#include "../Services/RTCService.hpp"
#include "../UIManager.hpp"

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
