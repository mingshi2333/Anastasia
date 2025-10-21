#include "wsi/wsi.h"
#include "event/event.h"
#include "event/input.h"
#include "wsi/keymap.h"
#include <GLFW/glfw3.h>
#include <algorithm>

namespace ana::wsi
{

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
                           [](GLFWwindow* w, int key, int, int action, int)
                           {
                               auto* self = static_cast<GLFWWSI*>(glfwGetWindowUserPointer(w));
                               if (!self || !self->sink)
                                   return;
                               Key k = ana::wsi::fromGlfwKey(key);
                               if (k == Key::Unknown)
                                   return;

                               self->sink(KeyboardEvent{ k, ana::wsi::fromGlfwAction(action) });
                           });
        glfwSetFramebufferSizeCallback(win,
                                       [](GLFWwindow* w, int width, int height)
                                       {
                                           auto* self = static_cast<GLFWWSI*>(glfwGetWindowUserPointer(w));
                                           if (!self || !self->resizeSink)
                                               return;
                                           const uint32_t clampedWidth  = static_cast<uint32_t>(std::max(width, 0));
                                           const uint32_t clampedHeight = static_cast<uint32_t>(std::max(height, 0));
                                           self->resizeSink(ana::WindowResizeEvent{ clampedWidth, clampedHeight });
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

    GLFWwindow* nativeHandle() const override
    {
        return win;
    }

private:
    GLFWwindow* win = nullptr;
};

// 简单工厂（外部链接）
std::unique_ptr<IWSI> CreateGLFWWSI(int w, int h, const char* title)
{
    return std::unique_ptr<IWSI>(new GLFWWSI(w, h, title));
}
} // namespace ana::wsi
