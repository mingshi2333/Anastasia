#pragma once

#include "device.h"

// vulkan headers
#include <fcntl.h>
#include <vulkan/vulkan.h>

// std lib headers
#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ana::vk
{

struct DynamicRenderingFotmat
{
    VkFormat colorAttachment;
    VkFormat depthAttachment;
};

class SwapChain
{
public:
    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

    SwapChain(vk::Device& deviceRef, VkExtent2D windowExtent);
    SwapChain(vk::Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
    ~SwapChain();

    SwapChain(const SwapChain&)      = delete;
    void operator=(const SwapChain&) = delete;
    SwapChain& operator=(SwapChain&) = delete;

    VkImageView getImageView(int index)
    {
        return swapChainImageViews[index];
    }

    VkImage getImage(int index)
    {
        return swapChainImages[index];
    }

    VkImageView getDepthImageView(int index)
    {
        return depthImageViews[index];
    }

    size_t imageCount()
    {
        return swapChainImages.size();
    }

    VkFormat getSwapChainImageFormat()
    {
        return swapChainImageFormat;
    }

    VkExtent2D getSwapChainExtent()
    {
        return swapChainExtent;
    }

    VkRenderPass getSwapChainRendererPass()
    {
        return swapChainRendererPass;
    }

    VkFramebuffer getFrameBuffer(int index)
    {
        return swapChainFramebuffers[index];
    }

    uint32_t width()
    {
        return swapChainExtent.width;
    }

    uint32_t height()
    {
        return swapChainExtent.height;
    }

    float extentAspectRatio()
    {
        return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }

    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t* imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

    bool compareSwapFormats(const SwapChain& swapChain) const
    {
        return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
               swapChain.swapChainImageFormat == swapChainImageFormat;
    }

private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();

    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkFormat swapChainImageFormat;
    VkFormat swapChainDepthFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass swapChainRendererPass;
    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    vk::Device& device;
    VkExtent2D windowExtent;

    VkSwapchainKHR swapChain;
    std::shared_ptr<SwapChain> oldSwapChain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
};

} // namespace ana::vk
