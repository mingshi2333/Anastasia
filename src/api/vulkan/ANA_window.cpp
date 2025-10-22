#include "ANA_window.h"
#include "event/event.h"
#include "event/input.h"
#include "wsi/wsi.h"
#include <stdexcept>

namespace ana
{

ANAwindow::ANAwindow(int w, int h, std::string name)
    : width(w)
    , height(h)
    , windowName(std::move(name))
{
    // Create WSI window and wire resize -> update cached extent
    wsi = ana::wsi::CreateGLFWWSI(width, height, windowName.c_str());
    if (!wsi)
    {
        throw std::runtime_error("failed to create WSI window");
    }
    wsi->setResizeSink(
        [this](const ana::WindowResizeEvent& e)
        {
            framebufferResized = true;
            width              = static_cast<int>(e.m_width);
            height             = static_cast<int>(e.m_height);
        });
    // Keyboard events are delivered via setKeySink() below (optional)
}

ANAwindow::~ANAwindow() = default;

void ANAwindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    auto* native = getGLFWwindow();
    if (!native)
    {
        throw std::runtime_error("failed to access native window");
    }
    if (glfwCreateWindowSurface(instance, native, nullptr, surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

bool ANAwindow::poll()
{
    return wsi ? wsi->poll() : false;
}

void ANAwindow::setKeySink(std::function<void(const ana::KeyboardEvent&)> sink)
{
    keySink = sink; // keep a copy if caller needs to query
    if (wsi)
    {
        wsi->setKeySink(std::move(sink));
    }
}

} // namespace ana
