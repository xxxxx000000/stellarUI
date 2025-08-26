#pragma once

#define UI_CORNER_RADIUS 30 // 屏幕圆角半径 rounded corners radius of the screen
#define SCREEN_ROTATION 0 // 屏幕旋转方向 screen rotation, 0-3

#define transitionDuration 400 // 页面切换动画时长 duration of page transition animation in milliseconds
#define popupDuration 400 // 弹窗动画时长 duration of popup animation in milliseconds
#define buttonDuration 400 // 按钮动画时长 duration of button animation in milliseconds
#define switchDuration 400 // 开关动画时长 duration of switch animation in milliseconds
#define startupDuration 1600 // 开机动画时长 duration of start up animation

#define AUTO_LOCK_TIME 10 // 自动锁屏时间，单位秒 auto lock time in seconds

#define batteryService // 是否启用电池服务 enable battery service
#define brightnessService // 是否启用亮度服务 enable brightness service
#define framerateService // 是否启用帧率服务 enable framerate service
#define imuService // 是否启用IMU服务 enable IMU service
#define rtcService // 是否启用RTC服务 enable RTC service
#define sleepService // 是否启用睡眠服务 enable sleep service
#define storageService // 是否启用存储服务 enable storage service


//在此处添加触摸函数，一个是初始化一个是单点触摸获取 add touch functions here, one for initialization and one for single-point touch acquisition
#include <TouchDrvCSTXXX.hpp>
static TouchDrvCST816 cst;
static void beginTouch() {
    Wire.begin(11, 10);
    cst.begin(Wire, 0x15, 11, 10);
    cst.disableAutoSleep();
}
static bool getPoint(int16_t *x_array, int16_t *y_array){
    return cst.getPoint(x_array, y_array, 1);
}



#define batteryVoltagePin 1 // 电池电压采样引脚 Battery voltage sampling pin (via resistor divider)
#define screenInterruptPin 14 // 触摸中断引脚，用于唤醒 Touch interrupt pin, used for wake-up
#define batteryEnablePin 41 // 电池使能引脚，不需要可注释掉 Battery enable pin, can be commented out if not needed

#define DUAL_CORE 1 // 是否启用双核运行 enable dual-core operation

// 页面统一管理，将所有页面头文件包含在此 Unified page management, include all page header files here
#include "Pages/mainPage.hpp"
#include "Pages/lockedPage.hpp"
#include "Pages/settimepage.hpp"
#include "Pages/posturepage.hpp"
#include "Pages/toolspage.hpp"
#include "Pages/wifipage.hpp"
#include "Pages/bluetoothpage.hpp"

#define FirstPage new LockedPage() // 启动之后显示的第一个页面（可更换） The first page to display after startup (can be changed)