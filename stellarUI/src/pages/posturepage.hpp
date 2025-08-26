#pragma once
#include "../UIComponents/page.hpp"
#include "../UIComponents/linechart.hpp"
#include "../UIComponents/button.hpp"
#include "../UIComponents/text.hpp"
#include "../UIComponents/livetext.hpp"
#include "../UIComponents/switch.hpp"
#include "../services/IMUService.hpp"
#include "../services/sleepservice.hpp"


class PosturePage : public Page {
public:
    PosturePage();

private:
    LineChartComponent* linechart = nullptr;
    LiveText* liveText = nullptr;
};