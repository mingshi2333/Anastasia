#pragma once

#include "api/pipeline.h"
#include "api/vulkan/ANA_window.h"
#include "api/vulkan/device.h"
#include "api/vulkan/model.h"
#include "api/vulkan/swapchain.h"
#include <memory>
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
    void sierpinski(std::vector<Model::Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);
    void loadModel();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    void initImGui();
    void renderImGui(VkCommandBuffer commandBuffer);
    void shutdownImGui();

    ANAwindow window{ WIDTH, HEIGHT, "Vulkan" };
    vk::Device device{ window };
    vk::SwapChain swapChain{ device, window.getExtent() };

    std::unique_ptr<vk::ANAPipeline> anaPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;

    VkDescriptorPool imguiPool = VK_NULL_HANDLE;
    std::unique_ptr<Model> model;
};
} // namespace ana
