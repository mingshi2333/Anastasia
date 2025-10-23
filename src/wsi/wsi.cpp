#include "wsi/wsi.h"
#include "event/event.h"
#include "event/input.h"
#include "wsi/keymap.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ana::wsi
{

class GLFWWSI final : public IWSI
{
public:
    GLFWWSI(int width, int height, const char* title)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwSetWindowUserPointer(window, this);

        // cache initial extent
        int fbWidth  = 0;
        int fbHeight = 0;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        extent = { static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight) };

        // cache initial cursor position
        glfwGetCursorPos(window, &lastCursorX, &lastCursorY);

        // keyboard
        glfwSetKeyCallback(window,
                           [](GLFWwindow* w, int key, int, int action, int mods)
                           {
                               auto* self = static_cast<GLFWWSI*>(glfwGetWindowUserPointer(w));
                               if (!self || !self->keySink)
                                   return;

                               (void)mods;

                               ana::Key k = ana::wsi::fromGlfwKey(key);
                               if (k == ana::Key::Unknown)
                                   return;

                               ana::KeyState s = ana::wsi::fromGlfwAction(action);
                               // TODO: handle mods if KeyboardEvent需要
                               self->keySink(ana::KeyboardEvent{ k, s });
                           });

        // mouse move
        glfwSetCursorPosCallback(window,
                                 [](GLFWwindow* w, double xpos, double ypos)
                                 {
                                     auto* self = static_cast<GLFWWSI*>(glfwGetWindowUserPointer(w));
                                     if (!self || !self->mouseMoveSink)
                                         return;

                                     double dx         = xpos - self->lastCursorX;
                                     double dy         = ypos - self->lastCursorY;
                                     self->lastCursorX = xpos;
                                     self->lastCursorY = ypos;

                                     self->mouseMoveSink(
                                         ana::MouseMoveEvent{ static_cast<float>(dx), static_cast<float>(dy),
                                                              static_cast<float>(xpos), static_cast<float>(ypos) });
                                 });

        // mouse button
        glfwSetMouseButtonCallback(window,
                                   [](GLFWwindow* w, int button, int action, int)
                                   {
                                       auto* self = static_cast<GLFWWSI*>(glfwGetWindowUserPointer(w));
                                       if (!self || !self->mouseButtonSink)
                                           return;

                                       ana::MouseButton btn = ana::MouseButton::Left;
                                       switch (button)
                                       {
                                       case GLFW_MOUSE_BUTTON_RIGHT:
                                           btn = ana::MouseButton::Right;
                                           break;
                                       case GLFW_MOUSE_BUTTON_MIDDLE:
                                           btn = ana::MouseButton::Middle;
                                           break;
                                       default:
                                           break;
                                       }

                                       double x = 0.0, y = 0.0;
                                       glfwGetCursorPos(w, &x, &y);
                                       self->mouseButtonSink(ana::MouseButtonEvent{
                                           btn, static_cast<float>(x), static_cast<float>(y), action == GLFW_PRESS });
                                   });

        // resize (framebuffer size change)
        glfwSetFramebufferSizeCallback(window,
                                       [](GLFWwindow* w, int width, int height)
                                       {
                                           auto* self = static_cast<GLFWWSI*>(glfwGetWindowUserPointer(w));
                                           if (!self)
                                               return;

                                           const uint32_t clampedW = width > 0 ? static_cast<uint32_t>(width) : 0;
                                           const uint32_t clampedH = height > 0 ? static_cast<uint32_t>(height) : 0;
                                           self->extent            = { clampedW, clampedH };

                                           if (self->resizeSink)
                                           {
                                               self->resizeSink(ana::WindowResizeEvent{ clampedW, clampedH });
                                           }
                                       });
    }

    ~GLFWWSI() override
    {
        if (window)
        {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        glfwTerminate();
    }

    bool poll() override
    {
        glfwPollEvents();
        return window && !glfwWindowShouldClose(window);
    }

    std::vector<const char*> getRequiredInstanceExtensions() const override
    {
        uint32_t count    = 0;
        const char** exts = glfwGetRequiredInstanceExtensions(&count);
        if (!exts || count == 0)
        {
            throw std::runtime_error("glfwGetRequiredInstanceExtensions returned no extensions");
        }
        return std::vector<const char*>(exts, exts + count);
    }

    VkSurfaceKHR createSurface(VkInstance instance) override
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan surface (GLFW)");
        }
        return surface;
    }

    VkExtent2D framebufferExtent() const override
    {
        return extent;
    }

    GLFWwindow* nativeHandle() const override
    {
        return window;
    }

private:
    GLFWwindow* window = nullptr;
    VkExtent2D extent{ 0, 0 };
    double lastCursorX = 0.0;
    double lastCursorY = 0.0;
};

std::unique_ptr<IWSI> CreateGLFWWSI(int width, int height, const char* title)
{
    return std::make_unique<GLFWWSI>(width, height, title);
}

} // namespace ana::wsi
