#pragma once
#include "event/input.h"
#include <GLFW/glfw3.h>

namespace ana::wsi
{

// Map GLFW key code to engine Key enum
inline ana::Key fromGlfwKey(int k)
{
    switch (k)
    {
    case GLFW_KEY_W:
        return ana::Key::W;
    case GLFW_KEY_A:
        return ana::Key::A;
    case GLFW_KEY_S:
        return ana::Key::S;
    case GLFW_KEY_D:
        return ana::Key::D;
    case GLFW_KEY_Q:
        return ana::Key::Q;
    case GLFW_KEY_E:
        return ana::Key::E;
    case GLFW_KEY_P:
        return ana::Key::P;
    case GLFW_KEY_ESCAPE:
        return ana::Key::Escape;
    case GLFW_KEY_LEFT:
        return ana::Key::Left;
    case GLFW_KEY_RIGHT:
        return ana::Key::Right;
    case GLFW_KEY_UP:
        return ana::Key::Up;
    case GLFW_KEY_DOWN:
        return ana::Key::Down;
    case GLFW_KEY_0:
        return ana::Key::_0;
    case GLFW_KEY_1:
        return ana::Key::_1;
    case GLFW_KEY_2:
        return ana::Key::_2;
    case GLFW_KEY_3:
        return ana::Key::_3;
    case GLFW_KEY_4:
        return ana::Key::_4;
    case GLFW_KEY_5:
        return ana::Key::_5;
    case GLFW_KEY_6:
        return ana::Key::_6;
    case GLFW_KEY_7:
        return ana::Key::_7;
    case GLFW_KEY_8:
        return ana::Key::_8;
    case GLFW_KEY_9:
        return ana::Key::_9;
    case GLFW_KEY_LEFT_SHIFT:
        return ana::Key::LeftShift;
    case GLFW_KEY_LEFT_CONTROL:
        return ana::Key::LeftCtrl;
    case GLFW_KEY_LEFT_ALT:
        return ana::Key::LeftAlt;
    case GLFW_KEY_SPACE:
        return ana::Key::Space;
    case GLFW_KEY_ENTER:
        return ana::Key::Return;
    default:
        return ana::Key::Unknown;
    }
}

inline ana::KeyState fromGlfwAction(int a)
{
    return a == GLFW_PRESS   ? ana::KeyState::Pressed :
           a == GLFW_RELEASE ? ana::KeyState::Released :
                               ana::KeyState::Repeat;
}

} // namespace ana::wsi
