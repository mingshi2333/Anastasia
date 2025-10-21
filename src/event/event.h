#pragma once

#include "common/hash.h"
#include "event.h"
#include "event/input.h"

// total 3 event

namespace ana
{

enum class EventType
{
    UNDEFINED,
    KEY,
    MOUSE_MOVE,
    MOUSE_BUTTON,
    WINDOW_RESIZE
};

class Event
{
public:
    explicit Event(EventType type)
        : m_type(type) {};

    virtual ~Event() = default;

private:
    EventType m_type = EventType::UNDEFINED;
};

struct MouseButtonEvent : public Event
{
    MouseButtonEvent(MouseButton button, float absX, float absY, bool pressed)
        : Event(EventType::MOUSE_BUTTON)
        , m_button(button)
        , m_absX(absX)
        , m_absY(absY)
        , m_pressed(pressed)
    {
    }

    MouseButton m_button;
    float m_absX;
    float m_absY;
    bool m_pressed;
};

struct MouseMoveEvent : public Event
{
    MouseMoveEvent(float deltX, float deltaY, float absX, float absY)
        : Event(EventType::MOUSE_MOVE)
        , m_deltaX(deltX)
        , m_deltaY(deltaY)
        , m_absX(absX)
        , m_absY(absY)
    {
    }

    float m_deltaX;
    float m_deltaY;
    float m_absX;
    float m_absY;
};

struct KeyboardEvent : public Event
{
    KeyboardEvent(Key key, KeyState keystate)
        : Event(EventType::KEY)
        , m_key(key)
        , m_keystate(keystate)
    {
    }

    Key m_key;
    KeyState m_keystate;
};

struct WindowResizeEvent : public Event
{
    explicit WindowResizeEvent(uint32_t width, uint32_t height)
        : Event(EventType::WINDOW_RESIZE)
        , m_width(width)
        , m_height(height)
    {
    }

    uint32_t m_width;
    uint32_t m_height;
};

} // namespace ana