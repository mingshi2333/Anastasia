#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include "event/event.h" // ana::KeyboardEvent
#include "wsi/wsi.h" // ana::wsi::IWSI
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
    void setKeySink(std::function<void(const ana::KeyboardEvent&)> sink);
    bool poll();

    // Back-compat alias for existing call sites
    bool pollEvents()
    {
        return poll();
    }

    bool shouldClose()
    {
        auto* h = getGLFWwindow();
        return h ? glfwWindowShouldClose(h) : true;
    }

    VkExtent2D getExtent()
    {
        return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    }

    GLFWwindow* getGLFWwindow() const
    {
        return wsi ? wsi->nativeHandle() : nullptr;
    }

    bool wasWindowResized()
    {
        return framebufferResized;
    }

    void resetWindowResizedFlag()
    {
        framebufferResized = false;
    }

private:
    std::unique_ptr<ana::wsi::IWSI> wsi;
    int width;
    int height;
    std::string windowName;
    bool framebufferResized = false;
    std::function<void(const ana::KeyboardEvent&)> keySink;
};

} // namespace ana
