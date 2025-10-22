#pragma once
#include "event/event.h"
#include "event/input.h"
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

namespace ana::wsi
{
class IWSI
{
public:
    using KeySink         = std::function<void(const ana::KeyboardEvent&)>;
    using MouseButtonSink = std::function<void(const ana::MouseButtonEvent&)>;
    using MouseMoveSink   = std::function<void(const ana::MouseMoveEvent&)>;
    using ResizeSink      = std::function<void(const ana::WindowResizeEvent&)>;

    virtual ~IWSI() = default;

    virtual bool poll() = 0;

    virtual std::vector<const char*> getRequiredInstanceExtensions() const = 0;

    virtual VkSurfaceKHR createSurface(VkInstance instance) = 0;
    virtual VkExtent2D frameBufferExtent() const            = 0;

    virtual GLFWwindow* nativeHandle() const = 0;

    void setKeySink(KeySink s)
    {
        sink = std::move(s);
    }

    void setResizeSink(ResizeSink s)
    {
        resizeSink = std::move(s);
    }

    void setMouseButtonSink(MouseButtonSink s)
    {
        mouseButtonSink = std::move(s);
    }

    void setMouseMoveSink(MouseMoveSink s)
    {
        mouseMoveSink = std::move(s);
    }

protected:
    KeySink sink;
    ResizeSink resizeSink;
    MouseButtonSink mouseButtonSink;
    MouseMoveSink mouseMoveSink;
};

// Factory (GLFW implementation). Header declares it so app.cpp can create one.
std::unique_ptr<IWSI> CreateGLFWWSI(int w, int h, const char* title);
} // namespace ana::wsi
