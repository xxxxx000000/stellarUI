#include <Arduino.h>
#include <TFT_eSPI.h>
#include <FreeRTOS.h>

#include <TouchDrvCSTXXX.hpp> // 根据实际更换为合适的触摸驱动 Replace with the appropriate touch driver as needed

// 页面统一管理，将所有页面头文件包含在此 Unified page management, include all page header files here
#include "Pages/mainPage.h"
#include "Pages/lockedPage.h"
#include "Pages/settimepage.h"
#include "Pages/posturepage.h"

// 各类服务模块，按需引入（建议保留 storageservice，brightnessservice 依赖它）Various service modules, include as needed (recommend keeping storageservice since brightnessservice depends on it)
#include "services/batteryservice.h"
#include "services/brightnessservice.h"
#include "services/framerateservice.h"
#include "services/imuservice.h"
#include "services/inputservice.h"
#include "services/rtcservice.h"
#include "services/sleepservice.h"
#include "services/storageservice.h"

#include "UIManager.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite buffer = TFT_eSprite(&tft);

TouchDrvCST816 cst; // 触摸驱动对象，替换为你的驱动类 Touch driver instance, replace with your own driver class

UIManager uiManager;

// 服务实例化，部分服务依赖硬件引脚，请根据实际情况调整 Service instantiation, some services require hardware pins, please adjust accordingly
BatteryService batteryservice(1);// 电压采样引脚（电阻分压），参数为引脚 Voltage sampling pin (via resistor divider), parameter is the pin number
BrightnessService brightnessservice;
FrameRateService framerateservice;
IMUService imuservice;
InputService inputservice;
RTCService rtcservice;
SleepService sleepservice;
StorageService storageservice("user");

TaskHandle_t displayTaskHandle = NULL;
TaskHandle_t logicTaskHandle = NULL;

void logicTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1); // 可根据需要调整执行周期（单位：毫秒）Can be adjusted as needed (unit: milliseconds)

    while (true) {
        uint32_t now = millis();
        
        // 这一段跟触摸有关的可替换为，个人的触摸驱动检测单点触摸的方法（只支持单点触摸），最后将触摸点的横纵坐标赋给tx，ty
        // Replace this section with your own single-point touch detection logic, assign coordinates to tx and ty
        int16_t x_array[1], y_array[1];
        bool touched = cst.getPoint(x_array, y_array, 1);
        if (touched) {
            sleepservice.touchEvent();
        }
        int16_t tx = touched ? x_array[0] : -1;
        int16_t ty = touched ? y_array[0] : -1;
        uiManager.handleTouch(tx, ty);
        uiManager.update(now);

        // 同理，不需要的服务可以删除 Similarly, remove unused services if not needed
        batteryservice.update();
        brightnessservice.update();
        imuservice.update();
        inputservice.update();
        sleepservice.update();

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void displayTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(10); // 控制最大帧率，避免过高或过低 Limit maximum frame rate to avoid being too high or too low

    while (true) {
        buffer.fillSprite(TFT_BLACK);
        uiManager.draw(buffer);
        buffer.pushSprite(0, 0);
        framerateservice.frameTick(); // 不需要获取帧率可删除 Remove if frame rate tracking is not needed

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void setup() {
    // 电池使能引脚，根据不同开发板可替换或者删除 Battery enable pin, can be changed or removed depending on your board
    pinMode(41, OUTPUT);
    digitalWrite(41, HIGH);

    Wire.begin(11, 10); // I2C初始化，引脚同样取决于开发板 Initialize I2C, pins depend on your board

    // 替换为自己触摸驱动的初始化方式，建议禁用自动睡眠
    // Replace with your touch driver initialization, disabling auto-sleep is recommended
    cst.begin(Wire, 0x15, 11, 10);
    cst.disableAutoSleep();

    // 不需要的服务可以删除，传入的参数可以参考具体的service文件里面
    // Remove services you don't need; refer to individual service files for constructor parameters
    storageservice.begin();
    rtcservice.begin(11, 10);
    brightnessservice.begin();
    imuservice.begin(Wire, QMI8658_I2C_ADDRESS, 11, 10);
    sleepservice.begin(10000, GPIO_NUM_14); // 引脚14为触摸屏中断引脚，可调整为自定义的中断引脚 Pin 14 is used as the touch interrupt pin; can be changed to a custom one
    batteryservice.begin();

    psramInit(); // 若无psram则删除 Remove this if your board has no PSRAM

    tft.init();
    tft.fillScreen(TFT_WHITE);
    tft.setRotation(0); // rotation可调节，范围0~3，对应四个方向 Screen rotation can be set from 0 to 3 (four directions)

    buffer.createSprite(240, 280); // 此处应当按照屏幕像素大小调节 Adjust according to your screen resolution

    uiManager.pushPage(new LockedPage(), TransitionType::None); // 启动之后显示的第一个页面（可更换） The first page to display after startup (can be changed)
    
    // 第5个参数为任务优先级，可调节；最后一个参数为执行任务的核心，ESP32S3支持双核所以两个任务绑定两个核心
    // The 5th parameter is task priority; the last is the core number (ESP32S3 supports dual-core, so tasks run on different cores)
    xTaskCreatePinnedToCore(logicTask, "LogicTask", 8192, NULL, 3, &logicTaskHandle, 0);
    xTaskCreatePinnedToCore(displayTask, "DisplayTask", 8192, NULL, 4, &displayTaskHandle, 1);
}

void loop() {
    // 留空，FreeRTOS任务已接管
    // Leave empty, FreeRTOS tasks take full control
}