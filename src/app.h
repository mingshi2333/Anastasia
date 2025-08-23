#pragma once

#include "api/gameobject.h"
#include "api/pipeline.h"
#include "api/vulkan/ANA_window.h"
#include "api/vulkan/device.h"
#include "api/vulkan/renderer.h"
#include "api/vulkan/swapchain.h"
#include "rendersystem.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
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
    void loadGameObjects();

    ANAwindow window{ WIDTH, HEIGHT, "Vulkan" };
    vk::Device device{ window };
    // std::shared_ptr<vk::SwapChain> swapChain;
    Renderer renderer{ window, device };
    std::unique_ptr<RenderSystem> renderSystem;
    std::vector<GameObject> gameObjects;
};
} // namespace ana
