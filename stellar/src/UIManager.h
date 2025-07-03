#pragma once
#include <vector>
#include "UIComponents/page.h"

enum class TransitionType {
    None,
    SlideLeft,
    SlideRight,
    SlideUp,
    SlideDown,
};

class UIManager {
private:
    std::vector<Page*> pageStack;
    Page* transitionFrom = nullptr;
    Page* transitionTo = nullptr;
    TransitionType transitionType = TransitionType::None;
    int transitionProgress = 0; // 0~100

public:
    ~UIManager() {
        for (auto p : pageStack) delete p;
        pageStack.clear();
        if (transitionFrom) delete transitionFrom;
        if (transitionTo) delete transitionTo;
    }

    void pushPage(Page* page, TransitionType type = TransitionType::None) {
        if (transitionType != TransitionType::None) return;

        if (type == TransitionType::None) {
            if (!pageStack.empty()) {
                delete pageStack.back();
                pageStack.pop_back();
            }
            pageStack.push_back(page);
            return;
        }

        transitionFrom = currentPage();
        transitionTo = page;
        transitionType = type;
        transitionProgress = 0;
    }

    void popPage(TransitionType type = TransitionType::None) {
        if (transitionType != TransitionType::None) return;
        if (pageStack.size() < 2) return;

        if (type == TransitionType::None) {
            delete pageStack.back();
            pageStack.pop_back();
            return;
        }

        transitionFrom = currentPage();
        transitionTo = pageStack[pageStack.size() - 2];
        transitionType = type;
        transitionProgress = 0;
    }

    Page* currentPage() {
        if (pageStack.empty()) return nullptr;
        return pageStack.back();
    }

    void update(uint32_t now) {
        if (transitionType != TransitionType::None) {
            transitionProgress += 6;
            if (transitionProgress >= 100) {
                if (transitionFrom) {
                    delete transitionFrom;
                    transitionFrom = nullptr;
                }

                if (transitionTo && (pageStack.empty() || pageStack.back() != transitionTo)) {
                    pageStack.push_back(transitionTo);
                }

                transitionTo = nullptr;
                transitionType = TransitionType::None;
                transitionProgress = 0;
            }
        } else {
            if (auto p = currentPage()) {
                p->update(now);
            }
        }
    }

    void draw(TFT_eSprite& buf) {
        if (transitionType == TransitionType::None) {
            if (auto p = currentPage()) {
                p->draw(buf);
            }
        } else {
            int w = buf.width();
            int h = buf.height();
            int progressPxX = (w * transitionProgress) / 100;
            int progressPxY = (h * transitionProgress) / 100;

            switch (transitionType) {
                case TransitionType::SlideLeft:
                    if (transitionFrom) transitionFrom->draw(buf, -progressPxX, 0);
                    if (transitionTo) transitionTo->draw(buf, w - progressPxX, 0);
                    break;

                case TransitionType::SlideRight:
                    if (transitionFrom) transitionFrom->draw(buf, progressPxX, 0);
                    if (transitionTo) transitionTo->draw(buf, -w + progressPxX, 0);
                    break;

                case TransitionType::SlideUp:
                    if (transitionFrom) transitionFrom->draw(buf, 0, -progressPxY);
                    if (transitionTo) transitionTo->draw(buf, 0, h - progressPxY);
                    break;

                case TransitionType::SlideDown:
                    if (transitionFrom) transitionFrom->draw(buf, 0, progressPxY);
                    if (transitionTo) transitionTo->draw(buf, 0, -h + progressPxY);
                    break;

                default:
                    break;
            }
        }
    }
    void handleTouch(int tx, int ty) {
        if (transitionType != TransitionType::None) return;
        if (auto p = currentPage()) {
            p->handleTouch(tx, ty);
        }
    }
};