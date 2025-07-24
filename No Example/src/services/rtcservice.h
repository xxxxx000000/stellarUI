#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SensorPCF85063.hpp>
#include <time.h>

/*
    主要用于提供rtc模块获取或者设置时间等方法，需要根据自己的开发板及传感器修改或者重写
    Mainly used to provide methods for obtaining or setting time via the RTC module
    Needs to be modified or rewritten according to your own development board and sensors
*/

class RTCService {
private:
    SensorPCF85063 rtc;

public:
    bool begin(uint8_t sdaPin, uint8_t sclPin) {
        return rtc.begin(Wire,sdaPin,sclPin);
    }

    void setDateTime(int year, int month, int day, int hour, int minute, int second) {
        rtc.setDateTime(year, month, day, hour, minute, second);
    }

    String getTimeString() {
        struct tm now;
        rtc.getDateTime(&now);
        char buf[9];
        sprintf(buf, "%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);
        return String(buf);
    }

    String getDateString() {
        struct tm now;
        rtc.getDateTime(&now);
        char buf[11];
        sprintf(buf, "%04d-%02d-%02d", now.tm_year, now.tm_mon, now.tm_mday);
        return String(buf);
    }

    void getTime(struct tm& out) {
        rtc.getDateTime(&out);
    }
};
