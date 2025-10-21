#pragma once
#include <string>
#include <functional>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "event/event.h" // ana::KeyboardEvent

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
    void setKeySink(std::function<void(const ana::KeyboardEvent&)> sink);
    bool poll();
    // Back-compat alias for existing call sites
    bool pollEvents() { return poll(); }

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
    GLFWwindow* window{nullptr};
    int width;
    int height;
    std::string windowName;
    bool framebufferResized = false;
    std::function<void(const ana::KeyboardEvent&)> keySink; // optional sink
};

} // namespace ana
