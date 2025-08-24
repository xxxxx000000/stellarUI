#pragma once
#include "Text.h"
#include "../services/rtcservice.h"
#include "../config.h"

/*
    从rtc服务获取时间并显示的组件，参数为横纵坐标，字体大小，颜色，是否以中心为准（布尔）
    Component that retrieves time from the RTC service and displays it, with parameters: X and Y coordinates, font size, color, and a boolean indicating whether the position is centered

*/

#ifdef rtcService
extern RTCService rtcservice;
#endif

class ClockText : public Text {
private:
    uint32_t lastUpdate = 0;
    uint32_t updateInterval = 1000;

public:
    ClockText(int _x, int _y, int _fontSize = 2, uint16_t _color = TFT_WHITE, bool _centered = false)
        : Text(_x, _y, "", _fontSize, _color, _centered) {}
    ClockText(float _fx, float _fy, int _fontSize = 2, uint16_t _color = TFT_WHITE, bool _centered = false)
        : Text(_fx, _fy, "", _fontSize, _color, _centered) {}

    void update(uint32_t now) override {
        if (now - lastUpdate > updateInterval) {
            #ifdef rtcService
            setText(rtcservice.getTimeString());
            #else
            setText("--:--:--");
            #endif
            lastUpdate = now;
        }
        Text::update(now);
    }
};
