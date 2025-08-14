#include "app.h"
#include "vk/include/vk_device.h" // New RHI device implementation

#include <stdexcept>

// ImGui headers are still needed, but their usage will change
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace ana
{
APP::APP()
{
    // Create the new RHI device
    m_rhiDevice = std::make_unique<vk::Device>();

    // We will re-implement these using the new RHI
    // initImGui();
}

APP::~APP()
{
    // The unique_ptr m_rhiDevice will handle destruction.
    // shutdownImGui();
}

void APP::run()
{
    while (!window.shouldClose())
    {
        glfwPollEvents();
        drawFrame();
    }
}

void APP::initImGui()
{
    // TODO: Re-implement with RHI
}

void APP::shutdownImGui()
{
    // TODO: Re-implement with RHI
}

void APP::renderImGui(rhi::IDeviceContext* context)
{
    // TODO: Re-implement with RHI
}

void APP::drawFrame()
{
    // TODO: Re-implement with RHI
    // The entire rendering logic will be replaced by calls to the RenderGraph
    // e.g.,
    // auto* context = m_rhiDevice->getImmediateContext();
    // context->beginFrame();
    // auto& graph = context->getRenderGraph();
    // ... add passes to graph ...
    // graph.compile();
    // graph.execute();
    // context->endFrame();
}

} // namespace ana
