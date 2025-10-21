#include "wsi/wsi.h"
#include "event/event.h"
#include "event/input.h"
#include <GLFW/glfw3.h>

namespace ana::wsi
{
static inline Key fromGlfwKey(int k)
{
    switch (k)
    {
    case GLFW_KEY_W:
        return Key::W;
    case GLFW_KEY_A:
        return Key::A;
    case GLFW_KEY_S:
        return Key::S;
    case GLFW_KEY_D:
        return Key::D;
    case GLFW_KEY_Q:
        return Key::Q;
    case GLFW_KEY_E:
        return Key::E;
    case GLFW_KEY_P:
        return Key::P;
    case GLFW_KEY_ESCAPE:
        return Key::Escape;
    case GLFW_KEY_LEFT:
        return Key::Left;
    case GLFW_KEY_RIGHT:
        return Key::Right;
    case GLFW_KEY_UP:
        return Key::Up;
    case GLFW_KEY_DOWN:
        return Key::Down;
    case GLFW_KEY_0:
        return Key::_0;
    case GLFW_KEY_1:
        return Key::_1;
    case GLFW_KEY_2:
        return Key::_2;
    case GLFW_KEY_3:
        return Key::_3;
    case GLFW_KEY_4:
        return Key::_4;
    case GLFW_KEY_5:
        return Key::_5;
    case GLFW_KEY_6:
        return Key::_6;
    case GLFW_KEY_7:
        return Key::_7;
    case GLFW_KEY_8:
        return Key::_8;
    case GLFW_KEY_9:
        return Key::_9;
    default:
        return Key::Unknown;
    }
}

static inline KeyState fromGlfwAction(int a)
{
    return a == GLFW_PRESS ? KeyState::Pressed : a == GLFW_RELEASE ? KeyState::Released : KeyState::Repeat;
}

class GLFWWSI final : public IWSI
{
public:
    GLFWWSI(int w, int h, const char* title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        win        = glfwCreateWindow(w, h, title, nullptr, nullptr);
        auto* self = this;
        glfwSetWindowUserPointer(win, self);
        glfwSetKeyCallback(win,
                           [](GLFWwindow* w, int key, int, int action, int mods)
                           {
                               auto* self = static_cast<GLFWWSI*>(glfwGetWindowUserPointer(w));
                               if (!self || !self->sink)
                                   return;
                               Key k = fromGlfwKey(key);
                               if (k == Key::Unknown)
                                   return;

                               self->sink(KeyboardEvent{ k, fromGlfwAction(action), mods });
                           });
    }

    ~GLFWWSI() override
    {
        if (win)
        {
            glfwDestroyWindow(win);
        }
        glfwTerminate();
    }

    bool poll() override
    {
        glfwPollEvents();
        return win && !glfwWindowShouldClose(win);
    }

private:
    GLFWwindow* win = nullptr;
};

// 简单工厂
inline std::unique_ptr<IWSI> CreateGLFWWSI(int w, int h, const char* title)
{
    return std::unique_ptr<IWSI>(new GLFWWSI(w, h, title));
}
} // namespace ana::wsi
