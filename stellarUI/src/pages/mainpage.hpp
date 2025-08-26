#pragma once
#include "../UIComponents/page.hpp"
#include "../UIComponents/clocktext.hpp"
#include "../UIComponents/colorblock.hpp"
#include "../UIComponents/gesturedetector.hpp"
#include "../UIComponents/slidebar.hpp"
#include "../UIComponents/button.hpp"
#include "../services/IMUService.hpp"
#include "../UIComponents/livetext.hpp"
#include "../UIComponents/popup.hpp"
#include "../UIComponents/icon.hpp"
#include "../services/brightnessservice.hpp"
#include "../services/sleepservice.hpp"
#include "../services/batteryservice.hpp"
#include "../services/inputservice.hpp"
#include "../services/screenrotationservice.hpp"
#include "../config.hpp"

class MainPage : public Page {
public:
    MainPage();
};