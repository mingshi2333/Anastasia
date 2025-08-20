#include "ANA_window.h"
#include <stdexcept>

namespace ana
{

ANAwindow::ANAwindow(int w, int h, std::string name)
    : width(w)
    , height(h)
    , windowName(name)
{
    initWindow();
}

ANAwindow::~ANAwindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void ANAwindow::initWindow()
{
    // glfw start
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(getGLFWwindow(), this);
    glfwSetFramebufferSizeCallback(getGLFWwindow(), framebufferResizeCallback);
}

void ANAwindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

void ANAwindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto anaWindow                = reinterpret_cast<ANAwindow*>(glfwGetWindowUserPointer(window));
    anaWindow->framebufferResized = true;
    anaWindow->width              = width;
    anaWindow->height             = height;
}
} // namespace ana