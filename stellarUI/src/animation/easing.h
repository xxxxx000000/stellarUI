#pragma once
#include <Arduino.h>

enum class EasingType {
    Linear,

    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,

    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,

    EaseInQuart,
    EaseOutQuart,
    EaseInOutQuart,

    EaseInQuint,
    EaseOutQuint,
    EaseInOutQuint,

    EaseInSine,
    EaseOutSine,
    EaseInOutSine,

    EaseInExpo,
    EaseOutExpo,
    EaseInOutExpo,

    EaseInCirc,
    EaseOutCirc,
    EaseInOutCirc,

    EaseInElastic,
    EaseOutElastic,
    EaseInOutElastic,

    EaseInBack,
    EaseOutBack,
    EaseInOutBack,

    EaseInBounce,
    EaseOutBounce,
    EaseInOutBounce,
};

inline float applyEasing(EasingType type, float t) {
    switch (type) {
        case EasingType::Linear:
            return t;
        case EasingType::EaseInQuad:
            return t * t;
        case EasingType::EaseOutQuad:
            return t * (2 - t);
        case EasingType::EaseInOutQuad:
            return (t < 0.5f) ? 2 * t * t : -1 + (4 - 2 * t) * t;
        case EasingType::EaseInCubic:
            return t * t * t;
        case EasingType::EaseOutCubic: {
            float p = t - 1;
            return p * p * p + 1;
        }
        case EasingType::EaseInOutCubic:
            if (t < 0.5f)
                return 4 * t * t * t;
            else {
                float p = 2 * t - 2;
                return 0.5f * p * p * p + 1;
            }
        case EasingType::EaseInQuart:
            return t * t * t * t;
        case EasingType::EaseOutQuart: {
            float p = t - 1;
            return 1 - p * p * p * p;
        }
        case EasingType::EaseInOutQuart:
            if (t < 0.5f)
                return 8 * t * t * t * t;
            else {
                float p = t - 1;
                return 1 - 8 * p * p * p * p;
            }
        case EasingType::EaseInQuint:
            return t * t * t * t * t;
        case EasingType::EaseOutQuint: {
            float p = t - 1;
            return p * p * p * p * p + 1;
        }
        case EasingType::EaseInOutQuint:
            if (t < 0.5f)
                return 16 * t * t * t * t * t;
            else {
                float p = 2 * t - 2;
                return 0.5f * p * p * p * p * p + 1;
            }
        case EasingType::EaseInSine:
            return 1 - cos((t * M_PI) / 2);
        case EasingType::EaseOutSine:
            return sin((t * M_PI) / 2);
        case EasingType::EaseInOutSine:
            return -0.5f * (cos(M_PI * t) - 1);
        case EasingType::EaseInExpo:
            return (t == 0) ? 0 : pow(2, 10 * (t - 1));
        case EasingType::EaseOutExpo:
            return (t == 1) ? 1 : 1 - pow(2, -10 * t);
        case EasingType::EaseInOutExpo:
            if (t == 0) return 0;
            if (t == 1) return 1;
            if (t < 0.5f) return 0.5f * pow(2, 20 * t - 10);
            return 1 - 0.5f * pow(2, -20 * t + 10);
        case EasingType::EaseInCirc:
            return 1 - sqrt(1 - t * t);
        case EasingType::EaseOutCirc:
            return sqrt((2 - t) * t);
        case EasingType::EaseInOutCirc:
            if (t < 0.5f)
                return 0.5f * (1 - sqrt(1 - 4 * t * t));
            else
                return 0.5f * (sqrt(-((2 * t) - 3) * ((2 * t) - 1)) + 1);
        case EasingType::EaseInElastic: {
            if (t == 0 || t == 1) return t;
            float p = 0.3f;
            float s = p / 4;
            float val = -pow(2, 10 * (t - 1)) * sin((t - 1 - s) * (2 * M_PI) / p);
            return val;
        }
        case EasingType::EaseOutElastic: {
            if (t == 0 || t == 1) return t;
            float p = 0.3f;
            float s = p / 4;
            float val = pow(2, -10 * t) * sin((t - s) * (2 * M_PI) / p) + 1;
            return val;
        }
        case EasingType::EaseInOutElastic: {
            if (t == 0 || t == 1) return t;
            float p = 0.45f;
            float s = p / 4;
            if (t < 0.5f) {
                float val = -0.5f * pow(2, 20 * t - 10) * sin((20 * t - 11.125f) * (2 * M_PI) / p);
                return val;
            } else {
                float val = 0.5f * pow(2, -20 * t + 10) * sin((20 * t - 11.125f) * (2 * M_PI) / p) + 1;
                return val;
            }
        }
        case EasingType::EaseInBack: {
            const float s = 1.70158f;
            return t * t * ((s + 1) * t - s);
        }
        case EasingType::EaseOutBack: {
            const float s = 1.70158f;
            float p = t - 1;
            return p * p * ((s + 1) * p + s) + 1;
        }
        case EasingType::EaseInOutBack: {
            const float s = 1.70158f * 1.525f;
            if (t < 0.5f) {
                return 0.5f * (t * 2) * (t * 2) * ((s + 1) * t * 2 - s);
            } else {
                float p = t * 2 - 2;
                return 0.5f * (p * p * ((s + 1) * p + s) + 2);
            }
        }
        case EasingType::EaseInBounce:
            return 1 - applyEasing(EasingType::EaseOutBounce, 1 - t);
        case EasingType::EaseOutBounce: {
            if (t < (1 / 2.75f)) {
                return 7.5625f * t * t;
            } else if (t < (2 / 2.75f)) {
                t -= (1.5f / 2.75f);
                return 7.5625f * t * t + 0.75f;
            } else if (t < (2.5f / 2.75f)) {
                t -= (2.25f / 2.75f);
                return 7.5625f * t * t + 0.9375f;
            } else {
                t -= (2.625f / 2.75f);
                return 7.5625f * t * t + 0.984375f;
            }
        }
        case EasingType::EaseInOutBounce:
            if (t < 0.5f)
                return applyEasing(EasingType::EaseInBounce, t * 2) * 0.5f;
            else
                return applyEasing(EasingType::EaseOutBounce, t * 2 - 1) * 0.5f + 0.5f;

        default:
            return t;
    }
}
