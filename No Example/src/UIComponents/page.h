#pragma once
#include <vector>
#include "component.h"
#include "animation/animator.h"

/*
    页面组件，用来容纳其他组件不需要参数
    Page component used to contain other components; no parameters required
*/

class Page : public Component {
public:
    std::vector<Component*> children;
    std::vector<Animator*> animators;

    Page(int _x = 0, int _y = 0) : Component(_x, _y) {}

    ~Page() {
        for (auto c : children) {
            delete c;
        }
        for (auto a : animators) {
            delete a;
        }
        children.clear();
        animators.clear();
    }

    void addChild(Component* c) {
        if (c) children.push_back(c);
    }

    void addAnimator(Animator* a) {
        if (a) animators.push_back(a);
    }

    void draw(TFT_eSprite& buf, int offsetX = 0, int offsetY = 0) override {
        if (!enabled) return;

        int totalOffsetX = x + offsetX;
        int totalOffsetY = y + offsetY;

        for (auto c : children) {
            if (c && c->enabled) {
                c->draw(buf, totalOffsetX, totalOffsetY);
            }
        }
    }

    void update(uint32_t now) override {
        Component::update(now);

        for (auto a : animators) {
            if (a) a->update(now);
        }

        for (auto c : children) {
            if (c && c->enabled) {
                c->update(now);
            }
        }
    }

    bool containsPoint(int px, int py) override {
        return true;
    }

    void handleTouch(int tx, int ty) override {
        if (!enabled) return;

        for (auto c : children) {
            if (c && c->enabled) {
                c->handleTouch(tx - x, ty - y);
            }
        }
    }

    void handlePress(int x, int y) override {
        for (auto c : children) {
            if (c && c->enabled) {
                c->onPress();
            }
        }
    }

    void handleRelease() override {
        for (auto c : children) {
            if (c && c->enabled) {
                c->onRelease();
            }
        }
    }
};
