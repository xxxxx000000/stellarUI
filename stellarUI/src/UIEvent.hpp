#pragma once
#include <Arduino.h>

enum class UIEventType {
    Programmatic,
    Touch,
};

struct UIEvent {
    UIEventType type;
    String customData;
};
