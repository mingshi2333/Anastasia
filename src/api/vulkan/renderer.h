#pragma once

#include "device.h"
#include "swapchain.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vulkan/vulkan_core.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Forward declarations for ImGui
struct ImGuiContext;

namespace ana
{
class Renderer
{
public:
    Renderer(ana::ANAwindow& window, vk::Device& device);
    ~Renderer();
    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(Renderer&&)      = delete;

    //getter
    bool isFrameInProgress() const
    {
        return isFrameStarted;
    }

    VkCommandBuffer getCurrentCommandBuffer() const
    {
        assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
        return commandBuffers[currentFrameIndex];
    }

    VkRenderPass getSwapChainRendererPass() const
    {
        return swapChain->getSwapChainRendererPass();
    }

    VkFormat getSwapChainImageFormat() const
    {
        return swapChain->getSwapChainImageFormat();
    }

    VkFormat getSwapChainDepthFormat() const
    {
        return swapChain->findDepthFormat();
    }

    float getAspectRatio() const
    {
        return swapChain->extentAspectRatio();
    }

    uint32_t getFrameIndex() const
    {
        assert(isFrameStarted && "can't call getFrameIndex when frame is not in progress");
        return currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRendererPass(VkCommandBuffer commandBuffer);
    void endSwapChainRendererPass(VkCommandBuffer commandBuffer);

private:
    void createCommandBuffers();
    void freeCommandBuffers();
    // void drawFrame();
    void recreateSwapChain();

    // void initImGui();
    // void RendererImGui(VkCommandBuffer commandBuffer);
    // void shutdownImGui();

    // void RendererGameObject(VkCommandBuffer commandBuffer);

    ANAwindow& window;
    vk::Device& device;
    std::shared_ptr<vk::SwapChain> swapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    uint32_t currentFrameIndex{ 0 };
    bool isFrameStarted{ false };

    VkDescriptorPool imguiPool = VK_NULL_HANDLE;
};
} // namespace ana
