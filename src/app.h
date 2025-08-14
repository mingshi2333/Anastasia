#pragma once

#include <memory>

// Forward declare RHI interfaces to reduce header dependencies
namespace ana::rhi {
    class IRenderDevice;
    class IDeviceContext;
}

// Keep the window for now, but we'll need to see its implementation
#include "api/vulkan/ANA_window.h" 

// Forward declarations for ImGui
struct ImGuiContext;

namespace ana
{
class APP
{
public:
    static constexpr int WIDTH  = 800;
    static constexpr int HEIGHT = 600;

    APP();
    ~APP();
    APP(const APP&)            = delete;
    APP& operator=(const APP&) = delete;
    APP(APP&&)                 = delete;
    APP& operator=(APP&&)      = delete;

    void run();

private:
    // These methods will be refactored or removed
    // void createPipelineLayout(); // Will be handled by RHI
    // void createPipeline(); // Will be handled by RHI
    // void createCommandBuffers(); // Will be handled by RHI
    void drawFrame();

    void initImGui();
    void renderImGui(rhi::IDeviceContext* context); // Changed signature
    void shutdownImGui();

    // Keep the window object for now
    ANAwindow window{ WIDTH, HEIGHT, "Vulkan" };

    // The new RHI device
    std::unique_ptr<rhi::IRenderDevice> m_rhiDevice;
};
} // namespace ana
