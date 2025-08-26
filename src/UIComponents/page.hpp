#pragma once
#include <vector>
#include "component.hpp"
#include "animation/animator.hpp"

/*
    页面组件，用来容纳其他组件不需要参数
    Page component used to contain other components; no parameters required
*/

class Page : public Component {
private:
    Component* activeComponent = nullptr;
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
        if (tx >= 0 && ty >= 0 && activeComponent == nullptr) {
            for (int i = children.size() - 1; i >= 0; --i) {
                Component* c = children[i];
                if (c && c->enabled) {
                    int localTx = tx - x;
                    int localTy = ty - y;
                    if (c->containsPoint(localTx, localTy)) {
                        activeComponent = c;
                        break;
                    }
                }
            }
        }

        bool isTouchEnd = (tx < 0 || ty < 0);

        if (activeComponent) {
            int localTx = tx - x;
            int localTy = ty - y;
            activeComponent->handleTouch(localTx, localTy);

            if (isTouchEnd) {
                activeComponent = nullptr;
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
