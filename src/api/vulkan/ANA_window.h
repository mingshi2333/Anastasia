#pragma once
#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>
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
    ANAwindow(std::unique_ptr<ana::wsi::IWSI> w)
        : wsi(std::move(w))
    {
    }

    ~ANAwindow()                           = default;
    ANAwindow(const ANAwindow&)            = delete;
    ANAwindow& operator=(const ANAwindow&) = delete;

    // void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    VkSurfaceKHR createSurface(VkInstance inst)
    {
        return wsi->createSurface(inst);
    }

    void setKeySink(std::function<void(const ana::KeyboardEvent&)> sink)
    {
        if (wsi)
        {
            wsi->setKeySink(std::move(sink));
        }
    }

    bool poll()
    {
        return wsi ? wsi->poll() : false;
    }

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
        return wsi->framebufferExtent();
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

    std::vector<const char*> getRequiredInstanceExtensions() const
    {
        return wsi->getRequiredInstanceExtensions();
    }

private:
    std::unique_ptr<ana::wsi::IWSI> wsi;
    // int width;
    // int height;
    // std::string windowName;
    bool framebufferResized = false;
    // std::function<void(const ana::KeyboardEvent&)> keySink;
};

} // namespace ana
