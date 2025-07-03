#pragma once
#include <Preferences.h>

/*
    存储读写服务，请勿在项目应用当中高频进行同样的读写以免损坏硬件，可自行拓展读写数据类型
    Storage read/write service
    Avoid frequent identical read/write operations in application to prevent hardware damage
    Data types for reading and writing can be extended as needed
*/

class StorageService {
private:
    Preferences preferences;
    const char* namespaceName;

public:
    StorageService(const char* ns = "user") : namespaceName(ns) {}

    void begin() {
        preferences.begin(namespaceName, false);
    }

    void end() {
        preferences.end();
    }

    void putUInt8(const char* key, uint8_t value) {
        preferences.putUChar(key, value);
    }

    uint8_t getUInt8(const char* key, uint8_t defaultValue = 0) {
        return preferences.getUChar(key, defaultValue);
    }

    void putInt(const char* key, int value) {
        preferences.putInt(key, value);
    }

    int getInt(const char* key, int defaultValue = 0) {
        return preferences.getInt(key, defaultValue);
    }
};
