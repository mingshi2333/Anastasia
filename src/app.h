#pragma once

#include "api/vulkan/ANA_window.h"
#include "api/pipeline.h"
#include "api/vulkan/device.h"
#include "api/vulkan/swapchain.h"
#include <memory>
#include <vulkan/vulkan_core.h>
namespace ana
{
class APP
{
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    APP();
    ~APP();
    APP(const APP&) = delete;
    APP& operator=(const APP&) = delete;
    APP(APP&&) = delete;
    APP& operator=(APP&&) = delete;

    void run();

private:
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    ANAwindow window{WIDTH, HEIGHT, "Vulkan"};
    vk::Device device{window};
    vk::SwapChain swapChain{device, window.getExtent()};

    std::unique_ptr<vk::ANAPipeline> anaPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
};
} // namespace ana