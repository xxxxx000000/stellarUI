#pragma once
#include "../UIComponents/page.h"
#include "../UIComponents/linechart.h"
#include "../UIComponents/button.h"
#include "../UIComponents/text.h"
#include "../UIComponents/livetext.h"
#include "../UIComponents/switch.h"
#include "mainpage.h"
#include "../services/IMUService.h"
#include "../services/sleepservice.h"


class PosturePage : public Page {
public:
    PosturePage();

private:
    LineChartComponent* linechart = nullptr;
    LiveText* liveText = nullptr;
};