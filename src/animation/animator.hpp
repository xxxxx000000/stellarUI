#pragma once
#include <Arduino.h>
#include <vector>
#include "Easing.hpp"

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
        uint32_t startDelay = 0;
        EasingType easing = EasingType::Linear;

        bool finished = false;
        bool started = false;
    };

private:
    std::vector<AnimChannel> channels;
    uint32_t startTime = 0;
    bool running = false;
    bool loop = false;

public:
    Animator() = default;

    void addFloatAnim(float* target, float from, float to, uint32_t duration, EasingType easing = EasingType::Linear, uint32_t delay = 0) {
        AnimChannel ch;
        ch.target = (void*)target;
        ch.valueType = ValueType::Float;
        ch.fromF = from;
        ch.toF = to;
        ch.duration = duration;
        ch.startDelay = delay;
        ch.easing = easing;
        ch.finished = false;
        ch.started = false;
        channels.push_back(ch);
    }

    void addIntAnim(int* target, int from, int to, uint32_t duration, EasingType easing = EasingType::Linear, uint32_t delay = 0) {
        AnimChannel ch;
        ch.target = (void*)target;
        ch.valueType = ValueType::Int;
        ch.fromI = from;
        ch.toI = to;
        ch.duration = duration;
        ch.startDelay = delay;
        ch.easing = easing;
        ch.finished = false;
        ch.started = false;
        channels.push_back(ch);
    }

    void start() {
        if (channels.empty()) return;
        startTime = millis();
        running = true;
        for (auto& ch : channels) {
            ch.finished = false;
            ch.started = false;
        }
    }

    void stop() {
        running = false;
    }

    bool isRunning() const {
        return running;
    }

    bool isFinished() const {
        if (!running) return true;
        for (const auto& ch : channels) {
            if (!ch.finished) return false;
        }
        return true;
    }

    void setLoop(bool enable) {
        loop = enable;
    }

    void update(uint32_t now) {
        if (!running || channels.empty()) return;

        bool allFinished = true;

        for (auto& ch : channels) {
            if (ch.finished || !ch.target) continue;

            uint32_t localTime = now - startTime;

            if (localTime < ch.startDelay) {
                allFinished = false;
                continue;
            }

            uint32_t elapsed = localTime - ch.startDelay;

            if (!ch.started) {
                if (ch.valueType == ValueType::Float)
                    *((float*)ch.target) = ch.fromF;
                else
                    *((int*)ch.target) = ch.fromI;
                ch.started = true;
            }

            if (elapsed >= ch.duration) {
                if (ch.valueType == ValueType::Float)
                    *((float*)ch.target) = ch.toF;
                else
                    *((int*)ch.target) = ch.toI;
                ch.finished = true;
                continue;
            }

            allFinished = false;

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

        if (allFinished) {
            if (loop) {
                start();
            } else {
                running = false;
            }
        }
    }
};
