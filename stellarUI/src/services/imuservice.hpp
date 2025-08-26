#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "SensorQMI8658.hpp"
#include "storageservice.hpp"

/*
    主要用于提供陀螺仪、加速度传感器提供的数据，需要根据自己的开发板及传感器修改或者重写
    Mainly used to provide data from the gyroscope and accelerometer sensors
    Needs to be modified or rewritten according to your own development board and sensors

*/

#define QMI8658_I2C_ADDRESS QMI8658_L_SLAVE_ADDRESS

class IMUService {
private:
    SensorQMI8658 imu;
    bool initialized = false;

    float accel[3] = {0};
    float gravity[3] = {0};
    float linearAccel[3] = {0};
    float gyro[3] = {0};
    float temperature = 0;

    const float alpha = 0.8f;

public:
    bool begin(TwoWire &wire = Wire, uint8_t address = QMI8658_I2C_ADDRESS, int sda = 11, int scl = 10) {
        if (!imu.begin(wire, address, sda, scl)) {
            return false;
        }

        imu.configAccelerometer(
            SensorQMI8658::ACC_RANGE_4G,
            SensorQMI8658::ACC_ODR_1000Hz,
            SensorQMI8658::LPF_MODE_0);
        imu.configGyroscope(
            SensorQMI8658::GYR_RANGE_256DPS,
            SensorQMI8658::GYR_ODR_896_8Hz,
            SensorQMI8658::LPF_MODE_3);

        imu.enableAccelerometer();
        imu.enableGyroscope();
        initialized = true;
        return true;
    }

    void update() {
        if (!initialized || !imu.getDataReady()) return;

        float ax, ay, az, gx, gy, gz;

        if (imu.getAccelerometer(ax, ay, az)) {
            accel[0] = ax;
            accel[1] = ay;
            accel[2] = az;
        }

        if (imu.getGyroscope(gx, gy, gz)) {
            gyro[0] = gx;
            gyro[1] = gy;
            gyro[2] = gz;
        }

        temperature = imu.getTemperature_C();

        for (int i = 0; i < 3; i++) {
            gravity[i] = alpha * gravity[i] + (1.0f - alpha) * accel[i];
            linearAccel[i] = accel[i] - gravity[i];
        }
    }
    void getAcceleration(float &x, float &y, float &z) const {
        x = accel[0];
        y = accel[1];
        z = accel[2];
    }
    void getLinearAcceleration(float &x, float &y, float &z) const {
        x = linearAccel[0];
        y = linearAccel[1];
        z = linearAccel[2];
    }

    float getLinearAccelerationMagnitude() const {
        return sqrt(
            linearAccel[0]*linearAccel[0] +
            linearAccel[1]*linearAccel[1] +
            linearAccel[2]*linearAccel[2]
        );
    }
    void getGyroscope(float &x, float &y, float &z) const {
        x = gyro[0];
        y = gyro[1];
        z = gyro[2];
    }
    float getTemperature() const {
        return temperature;
    }
    float getAccelerationMagnitude() const {
        return sqrt(
            accel[0]*accel[0] +
            accel[1]*accel[1] +
            accel[2]*accel[2]
        );
    }

    bool isReady() const {
        return initialized;
    }
};
