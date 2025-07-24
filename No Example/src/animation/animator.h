#pragma once
#include <Arduino.h>
#include <vector>
#include "Easing.h"

class Animator {
public:
    enum class ValueType { Float, Int };

    struct AnimChannel {
        void* target = nullptr;
        ValueType valueType = ValueType::Float;
        float fromF = 0.0f;
        float toF = 0.0f;
        int fromI = 0;
        int toI = 0;
        uint32_t duration = 0;
        EasingType easing = EasingType::Linear;
    };

private:
    std::vector<AnimChannel> channels;
    int currentIndex = 0;
    uint32_t startTime = 0;
    bool running = false;
    bool loop = false;

public:
    Animator() = default;

    void addFloatAnim(float* target, float from, float to, uint32_t duration, EasingType easing = EasingType::Linear) {
        AnimChannel ch;
        ch.target = (void*)target;
        ch.valueType = ValueType::Float;
        ch.fromF = from;
        ch.toF = to;
        ch.duration = duration;
        ch.easing = easing;
        channels.push_back(ch);
    }

    void addIntAnim(int* target, int from, int to, uint32_t duration, EasingType easing = EasingType::Linear) {
        AnimChannel ch;
        ch.target = (void*)target;
        ch.valueType = ValueType::Int;
        ch.fromI = from;
        ch.toI = to;
        ch.duration = duration;
        ch.easing = easing;
        channels.push_back(ch);
    }

    void start() {
        if (channels.empty()) return;
        currentIndex = 0;
        startTime = millis();
        running = true;
        auto& ch = channels[currentIndex];
        if (!ch.target) return;
        if (ch.valueType == ValueType::Float)
            *((float*)ch.target) = ch.fromF;
        else
            *((int*)ch.target) = ch.fromI;
    }

    void stop() {
        running = false;
    }

    bool isRunning() const {
        return running;
    }

    void setLoop(bool enable) {
        loop = enable;
    }

    void update(uint32_t now) {
        if (!running || channels.empty()) return;

        auto& ch = channels[currentIndex];
        if (!ch.target) return;

        uint32_t elapsed = now - startTime;
        if (elapsed >= ch.duration) {
            if (ch.valueType == ValueType::Float)
                *((float*)ch.target) = ch.toF;
            else
                *((int*)ch.target) = ch.toI;

            currentIndex++;
            if (currentIndex >= (int)channels.size()) {
                if (loop) {
                    currentIndex = 0;
                } else {
                    running = false;
                    return;
                }
            }
            startTime = now;

            auto& nextCh = channels[currentIndex];
            if (nextCh.target) {
                if (nextCh.valueType == ValueType::Float)
                    *((float*)nextCh.target) = nextCh.fromF;
                else
                    *((int*)nextCh.target) = nextCh.fromI;
            }
        } else {
            float progress = (float)elapsed / ch.duration;
            float eased = applyEasing(ch.easing, progress);

            if (ch.valueType == ValueType::Float) {
                float val = ch.fromF + (ch.toF - ch.fromF) * eased;
                *((float*)ch.target) = val;
            } else {
                int val = ch.fromI + (int)((ch.toI - ch.fromI) * eased);
                *((int*)ch.target) = val;
            }
        }
    }
};
