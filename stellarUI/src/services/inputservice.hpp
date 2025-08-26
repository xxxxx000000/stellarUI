#pragma once
#include <Arduino.h>
#include <vector>
#include "../UIComponents/component.hpp"
#include "UIEvent.hpp"

/*
    用于将图形按钮绑定物理按键输入，目前支持上拉输入的按钮，按需修改或优化
    Used to bind graphical buttons to physical key inputs, currently supports buttons with pull-up input
    Modify or optimize as needed

*/

class InputService {
private:
    struct InputEntry {
        int pin;
        Component* component;
        int inputType;
        String customEventName;

        InputEntry(int p, Component* c, int type, const String& evtName)
            : pin(p), component(c), inputType(type), customEventName(evtName) {}
    };

    std::vector<InputEntry> inputs;

public:
    void registerInput(int pin, Component* comp, int inputType, const String& eventName) {
        pinMode(pin, INPUT_PULLUP);
        inputs.emplace_back(pin, comp, inputType, eventName);
    }

    void update() {
        for (auto& entry : inputs) {
            bool state = digitalRead(entry.pin);
            if (entry.component) {
                static std::vector<bool> lastStates(inputs.size(), HIGH);
                static bool inited = false;
                if (!inited) {
                    for (size_t i=0; i<inputs.size(); i++) lastStates[i] = HIGH;
                    inited = true;
                }

                size_t idx = &entry - &inputs[0];
                if (lastStates[idx] == HIGH && state == LOW) {
                    UIEvent e;
                    e.type = UIEventType::Programmatic;
                    e.customData = entry.customEventName;
                    entry.component->handleEvent(e);
                }
                lastStates[idx] = state;
            }
        }
    }
};
