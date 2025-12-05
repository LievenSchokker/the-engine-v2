#pragma once

#include "Events/Event.h"
#include <cstdint>
#include <cstring>


struct WindowResizeEvent : public Event<WindowResizeEvent>
{
    int width;
    int height;

    WindowResizeEvent(int w, int h)
        : width(w), height(h) {}
};

struct WindowCloseEvent : public Event<WindowCloseEvent>
{
    WindowCloseEvent() = default;
};

struct WindowFocusEvent : public Event<WindowFocusEvent>
{
    bool focused;

    explicit WindowFocusEvent(bool focused)
        : focused(focused) {}
};

struct KeyPressedEvent : public Event<KeyPressedEvent>
{
    int keyCode;
    int scanCode;
    bool isRepeat;

    KeyPressedEvent(int keyCode, int scanCode, bool repeat = false)
        : keyCode(keyCode), scanCode(scanCode), isRepeat(repeat) {}
};

struct KeyReleasedEvent : public Event<KeyReleasedEvent>
{
    int keyCode;
    int scanCode;

    KeyReleasedEvent(int keyCode, int scanCode)
        : keyCode(keyCode), scanCode(scanCode) {}
};

struct TextInputEvent : public Event<TextInputEvent>
{
    char text[32];

    explicit TextInputEvent(const char* inputText)
    {
        std::strncpy(text, inputText, sizeof(text) - 1);
        text[sizeof(text) - 1] = '\0';
    }
};

struct MouseMovedEvent : public Event<MouseMovedEvent>
{
    int x;
    int y;
    int deltaX;
    int deltaY;

    MouseMovedEvent(int x, int y, int dx, int dy)
        : x(x), y(y), deltaX(dx), deltaY(dy) {}
};

struct MouseButtonPressedEvent : public Event<MouseButtonPressedEvent>
{
    int button;
    int x;
    int y;
    int clicks;  // 1 = single, 2 = double click

    MouseButtonPressedEvent(int button, int x, int y, int clicks = 1)
        : button(button), x(x), y(y), clicks(clicks) {}
};

struct MouseButtonReleasedEvent : public Event<MouseButtonReleasedEvent>
{
    int button;
    int x;
    int y;

    MouseButtonReleasedEvent(int button, int x, int y)
        : button(button), x(x), y(y) {}
};

struct MouseScrollEvent : public Event<MouseScrollEvent>
{
    float deltaX;
    float deltaY;

    MouseScrollEvent(float dx, float dy)
        : deltaX(dx), deltaY(dy) {}
};

struct CollisionEvent : public Event<CollisionEvent>
{
    uint32_t entityA;
    uint32_t entityB;
    float impactForce;

    CollisionEvent(uint32_t a, uint32_t b, float force = 0.0f)
        : entityA(a), entityB(b), impactForce(force) {}
};

struct EntityDestroyedEvent : public Event<EntityDestroyedEvent>
{
    uint32_t entityId;

    explicit EntityDestroyedEvent(uint32_t id)
        : entityId(id) {}
};

struct SceneChangedEvent : public Event<SceneChangedEvent>
{
    uint32_t previousSceneId;
    uint32_t newSceneId;

    SceneChangedEvent(uint32_t previous, uint32_t next)
        : previousSceneId(previous), newSceneId(next) {}
};

