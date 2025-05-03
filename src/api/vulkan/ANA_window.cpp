#include "ANA_window.h"
#include <stdexcept>

namespace ana
{

ANAwindow::ANAwindow(int w, int h, std::string name) : width(w), height(h), windowName(name)
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

void ANAwindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}
} // namespace ana