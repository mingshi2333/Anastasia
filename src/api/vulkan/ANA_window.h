#pragma once
#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ana
{
class ANAwindow
{
public:
    ANAwindow(int width, int height, std::string windowName);
    ~ANAwindow();
    ANAwindow(const ANAwindow&)            = delete;
    ANAwindow& operator=(const ANAwindow&) = delete;
    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    void cleanup();

    bool shouldClose()
    {
        return glfwWindowShouldClose(window);
    }

    VkExtent2D getExtent()
    {
        return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    }

    GLFWwindow* getGLFWwindow() const
    {
        return window;
    }

    bool wasWindowResized()
    {
        return framebufferResized;
    }

    void resetWindowResizedFlag()
    {
        framebufferResized = false;
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
    void initWindow();
    GLFWwindow* window;
    int width;
    int height;
    std::string windowName;
    bool framebufferResized = false;
};

} // namespace ana