#pragma once
#include <vector>
#include <cmath>
#include "config.h"
#include "UIComponents/page.h"

enum class TransitionType {
    None,
    SlideLeft,
    SlideRight,
    SlideUp,
    SlideDown,
    CoverLeft,
    CoverRight,
    CoverUp,
    CoverDown,
    UncoverLeft,
    UncoverRight,
    UncoverUp,
    UncoverDown
};

class UIManager {
private:
    std::vector<Page*> pageStack;
    Page* transitionFrom = nullptr;
    Page* transitionTo = nullptr;
    TransitionType transitionType = TransitionType::None;

    uint32_t transitionStartTime = 0;
    static constexpr uint32_t transitionduration = transitionDuration;
    bool popping = false;
    bool transitionDone = false;

    float easeOutQuad(float t) {
        float c1 = 1.2f;
        float c3 = c1 + 1.0f;
        return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);
    }

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
        } else {
            transitionFrom = currentPage();
            transitionTo = page;
            transitionType = type;
            transitionStartTime = millis();
            popping = false;
            transitionDone = false;
        }
    }

    void popPage(TransitionType type = TransitionType::None) {
        if (transitionType != TransitionType::None) return;
        if (pageStack.size() < 2) return;

        if (type == TransitionType::None) {
            delete pageStack.back();
            pageStack.pop_back();
        } else {
            transitionFrom = currentPage();
            transitionTo = pageStack[pageStack.size() - 2];
            transitionType = type;
            transitionStartTime = millis();
            popping = true;
            transitionDone = false;
        }
    }

    Page* currentPage() {
        if (pageStack.empty()) return nullptr;
        return pageStack.back();
    }

    void update(uint32_t now) {
        now = millis();
        if (transitionType == TransitionType::None) {
            if (auto p = currentPage()) {
                p->update(now);
            }
        } else {
            uint32_t elapsed = now - transitionStartTime;
            if (elapsed >= transitionDuration) {
                transitionDone = true;
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

            float t = (millis() - transitionStartTime) / (float)transitionDuration;
            if (t > 1.0f) t = 1.0f;
            float easedT = easeOutQuad(t);

            int progressPxX = (int)(w * easedT);
            int progressPxY = (int)(h * easedT);

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

                case TransitionType::CoverLeft:
                    if (transitionFrom) transitionFrom->draw(buf, progressPxX/4, 0);
                    if (transitionTo) transitionTo->draw(buf, -w + progressPxX, 0);
                    break;

                case TransitionType::CoverRight:
                    if (transitionFrom) transitionFrom->draw(buf, - progressPxX/4, 0);
                    if (transitionTo) transitionTo->draw(buf, w - progressPxX, 0);
                    break;

                case TransitionType::CoverUp:
                    if (transitionFrom) transitionFrom->draw(buf, 0, progressPxY/4);
                    if (transitionTo) transitionTo->draw(buf, 0, -h + progressPxY);
                    break;

                case TransitionType::CoverDown:
                    if (transitionFrom) transitionFrom->draw(buf, 0, - progressPxY/4);
                    if (transitionTo) transitionTo->draw(buf, 0, h - progressPxY);
                    break;

                case TransitionType::UncoverLeft:
                    if (transitionTo) transitionTo->draw(buf,(w - progressPxX)/4, 0);
                    if (transitionFrom) transitionFrom->draw(buf, -progressPxX, 0);
                    break;

                case TransitionType::UncoverRight:
                    if (transitionTo) transitionTo->draw(buf, (-w + progressPxX)/4, 0);
                    if (transitionFrom) transitionFrom->draw(buf, progressPxX, 0);
                    break;

                case TransitionType::UncoverUp:
                    if (transitionTo) transitionTo->draw(buf, 0, (-h + progressPxY)/4);
                    if (transitionFrom) transitionFrom->draw(buf, 0, progressPxY);
                    break;

                case TransitionType::UncoverDown:
                    if (transitionTo) transitionTo->draw(buf, 0, (h - progressPxY)/4);
                    if (transitionFrom) transitionFrom->draw(buf, 0, -progressPxY);
                    break;

                default:
                    break;
            }

            if (transitionDone) {
                if (transitionFrom) {
                    delete transitionFrom;
                    transitionFrom = nullptr;
                }

                if (popping) {
                    delete pageStack.back();
                    pageStack.pop_back();
                } else {
                    if (!pageStack.empty() && pageStack.back() != transitionTo) {
                        pageStack.push_back(transitionTo);
                    } else {
                        delete transitionTo;
                    }
                }

                transitionTo = nullptr;
                transitionType = TransitionType::None;
                popping = false;
                transitionDone = false;
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
