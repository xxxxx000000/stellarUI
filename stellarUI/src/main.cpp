#include <Arduino.h>
#include <TFT_eSPI.h>
#include <FreeRTOS.h>
#include "config.h"

#ifdef batteryService
#include "services/batteryservice.h"
#endif
#ifdef brightnessService 
#include "services/brightnessservice.h"
#include "services/storageservice.h"
#endif
#ifdef framerateService
#include "services/framerateservice.h"
#endif
#ifdef imuService
#include "services/imuservice.h"
#endif
#ifdef rtcService
#include "services/rtcservice.h"
#endif
#ifdef sleepService
#include "services/sleepservice.h"
#include "services/brightnessservice.h"
#include "services/storageservice.h"
#endif
#ifdef storageService
#include "services/storageservice.h"
#endif

#include "UIManager.h"
#include "animation/bootanimation.h"
#include "services/inputservice.h"
#include "services/screenrotationservice.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite buffer = TFT_eSprite(&tft);
int16_t convertTouchX(int16_t x, int16_t y, uint8_t rotation, int width, int height);
int16_t convertTouchY(int16_t x, int16_t y, uint8_t rotation, int width, int height);

UIManager uiManager;
ScreenRotationService screenrotationservice;

#ifdef batteryService
BatteryService batteryservice(batteryVoltagePin);
#endif
#ifdef brightnessService 
BrightnessService brightnessservice;
#ifndef storageService
StorageService storageservice("user");
#endif
#endif
#ifdef framerateService
FrameRateService framerateservice;
#endif
#ifdef imuService
IMUService imuservice;
#endif
InputService inputservice;
#ifdef rtcService
RTCService rtcservice;
#endif
#ifdef sleepService
SleepService sleepservice;
#ifndef storageService
StorageService storageservice("user");
#endif
#ifndef brightnessService
BrightnessService brightnessservice;
#endif
#endif
#ifdef storageService
StorageService storageservice("user");
#endif

TaskHandle_t displayTaskHandle = NULL;
TaskHandle_t logicTaskHandle = NULL;

void logicTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1);

    while (true) {
        uint32_t now = millis();
        int16_t x_array[1], y_array[1];
        bool touched = getPoint(x_array, y_array);
        #ifdef sleepService
        if (touched) {
            sleepservice.touchEvent();
        }
        #endif
        int16_t tx = touched ? x_array[0] : -1;
        int16_t ty = touched ? y_array[0] : -1;

        int16_t rawX = tx;
        int16_t rawY = ty;

        tx = touched ? convertTouchX(rawX, rawY, screenrotationservice.getRotation(), tft.width(), tft.height()) : -1;
        ty = touched ? convertTouchY(rawX, rawY, screenrotationservice.getRotation(), tft.width(), tft.height()) : -1;

        uiManager.handleTouch(tx, ty);
        uiManager.update(now);

        #ifdef batteryService
        batteryservice.update();
        #endif
        #ifdef brightnessService
        brightnessservice.update();
        #endif
        #ifdef imuService
        imuservice.update();
        #endif
        inputservice.update();
        #ifdef sleepService
        brightnessservice.update();
        sleepservice.update();
        #endif

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void displayTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1);

    while (true) {
        buffer.fillScreen(TFT_BLACK);
        uiManager.draw(buffer);
        buffer.pushSprite(0, 0);
        #ifdef framerateService
        framerateservice.frameTick();
        #endif

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void setup() {
    #ifdef batteryEnablePin
    pinMode(batteryEnablePin, OUTPUT);
    digitalWrite(batteryEnablePin, HIGH);
    #endif

    beginTouch();

    tft.init();
    tft.fillScreen(TFT_BLACK);
    screenrotationservice.setRotation(SCREEN_ROTATION);

    buffer.createSprite(tft.width(), tft.height());

    BootAnimation2(tft, buffer);

    #ifdef storageService
    storageservice.begin();
    #endif
    #ifdef rtcService
    rtcservice.begin(11, 10);
    #endif
    #ifdef brightnessService
    storageservice.begin();
    brightnessservice.begin();
    #endif
    #ifdef imuService
    imuservice.begin(Wire, QMI8658_I2C_ADDRESS, 11, 10);
    #endif
    #ifdef sleepService
    storageservice.begin();
    brightnessservice.begin();
    sleepservice.begin(AUTO_LOCK_TIME * 1000, (gpio_num_t)screenInterruptPin);
    #endif
    #ifdef batteryService
    batteryservice.begin();
    #endif

    psramInit();

    
    uiManager.pushPage(FirstPage, TransitionType::None);
    
    #if DUAL_CORE == 1
    xTaskCreatePinnedToCore(logicTask, "LogicTask", 8192, NULL, 3, &logicTaskHandle, 0);
    xTaskCreatePinnedToCore(displayTask, "DisplayTask", 8192, NULL, 4, &displayTaskHandle, 1);
    #else
    xTaskCreate(logicTask, "LogicTask", 8192, NULL, 3, &logicTaskHandle);
    xTaskCreate(displayTask, "DisplayTask", 8192, NULL, 4, &displayTaskHandle);
    #endif
}

void loop() {
    
}

int16_t convertTouchX(int16_t x, int16_t y, uint8_t rotation, int width, int height) {
    switch (rotation) {
        case 0: return x;
        case 1: return y;
        case 2: return width - 1 - x;
        case 3: return height - 1 - y;
    }
    return x;
}

int16_t convertTouchY(int16_t x, int16_t y, uint8_t rotation, int width, int height) {
    switch (rotation) {
        case 0: return y;
        case 1: return height - 1 - x;
        case 2: return height - 1 - y;
        case 3: return x;
    }
    return y;
}
