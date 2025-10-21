#include "ANA_window.h"
#include "event/event.h"
#include "event/input.h"
#include "wsi/keymap.h"
#include <stdexcept>

namespace ana
{

ANAwindow::ANAwindow(int w, int h, std::string name)
    : width(w)
    , height(h)
    , windowName(std::move(name))
{
    // Initialize GLFW window
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    if (!window)
    {
        throw std::runtime_error("failed to create GLFW window");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    // Keyboard callback -> forward to sink if set
    glfwSetKeyCallback(window,
                       [](GLFWwindow* w, int key, int, int action, int /*mods*/)
                       {
                           auto* self = reinterpret_cast<ANAwindow*>(glfwGetWindowUserPointer(w));
                           if (!self || !self->keySink)
                               return;

                           ana::Key k = ana::wsi::fromGlfwKey(key);
                           if (k == ana::Key::Unknown)
                               return;

                           ana::KeyState s = ana::wsi::fromGlfwAction(action);
                           self->keySink(ana::KeyboardEvent{ k, s });
                       });
}

ANAwindow::~ANAwindow()
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void ANAwindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

void ANAwindow::framebufferResizeCallback(GLFWwindow* w, int ww, int hh)
{
    auto anaWindow                = reinterpret_cast<ANAwindow*>(glfwGetWindowUserPointer(w));
    anaWindow->framebufferResized = true;
    anaWindow->width              = ww;
    anaWindow->height             = hh;
}

bool ANAwindow::poll()
{
    glfwPollEvents();
    return window && !glfwWindowShouldClose(window);
}

} // namespace ana
namespace ana {
void ANAwindow::setKeySink(std::function<void(const ana::KeyboardEvent&)> sink)
{
    keySink = std::move(sink);
}
} // namespace ana
