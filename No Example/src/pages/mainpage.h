#pragma once
#include "../UIComponents/page.h"
#include "../UIComponents/clocktext.h"
#include "../UIComponents/colorblock.h"
#include "../UIComponents/gesturedetector.h"
#include "../UIComponents/slidebar.h"
#include "../UIComponents/button.h"
#include "../services/IMUService.h"
#include "../UIComponents/livetext.h"
#include "../UIComponents/popup.h"
#include "../UIComponents/icon.h"
#include "../services/brightnessservice.h"
#include "../services/sleepservice.h"
#include "../services/batteryservice.h"
#include "../services/inputservice.h"
#include "../services/screenrotationservice.h"

class MainPage : public Page {
public:
    MainPage();
};