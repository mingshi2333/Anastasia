#pragma once

#include "api/gameobject.h"
#include "api/pipeline.h"
#include "api/vulkan/device.h"
#include "api/vulkan/renderer.h"
#include "api/vulkan/swapchain.h"
#include "rendersystem.h"
#include "wsi/wsi.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

// Forward declarations for ImGui

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

    std::unique_ptr<ana::wsi::IWSI> wsi;
    std::unique_ptr<vk::Device> device;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<RenderSystem> renderSystem;
    std::vector<GameObject> gameObjects;
};
} // namespace ana
