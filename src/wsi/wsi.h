#pragma once
#include "event/event.h"
#include "event/input.h"
#include <functional>
#include <memory>

struct GLFWwindow;

namespace ana::wsi
{
class IWSI
{
public:
    using KeySink                            = std::function<void(const ana::KeyboardEvent&)>;
    using ResizeSink                         = std::function<void(const ana::WindowResizeEvent&)>;
    virtual ~IWSI()                          = default;
    virtual bool poll()                      = 0; // 拉取平台事件；返回 false 表示应退出
    virtual GLFWwindow* nativeHandle() const = 0;

    void setKeySink(KeySink s)
    {
        sink = std::move(s);
    }

    void setResizeSink(ResizeSink s)
    {
        resizeSink = std::move(s);
    }

protected:
    KeySink sink;
    ResizeSink resizeSink;
};

// Factory (GLFW implementation). Header declares it so app.cpp can create one.
std::unique_ptr<IWSI> CreateGLFWWSI(int w, int h, const char* title);
} // namespace ana::wsi
