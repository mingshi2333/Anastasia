#include "app.h"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace ana
{
APP::APP()
{
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

APP::~APP()
{
    vkDeviceWaitIdle(device.device());
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void APP::run()
{
    while (!window.shouldClose())
    {
        glfwPollEvents();
        drawFrame();
    }
}

void APP::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 0;
    pipelineLayoutInfo.pSetLayouts            = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges    = nullptr;
    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void APP::createPipeline()
{
    auto pipelineConfig           = vk::ANAPipeline::defaultPipelineConfigInfo(swapChain.width(), swapChain.height());
    pipelineConfig.renderPass     = swapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    anaPipeline =
        std::make_unique<vk::ANAPipeline>(device, "../shaders/vert.spv", "../shaders/frag.spv", pipelineConfig);
}

void APP::createCommandBuffers()
{
    commandBuffers.resize(swapChain.imageCount());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //?
    allocInfo.commandPool        = device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    for (int i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass            = swapChain.getRenderPass();
        renderPassInfo.framebuffer           = swapChain.getFrameBuffer(i);
        renderPassInfo.renderArea.offset     = { 0, 0 };
        renderPassInfo.renderArea.extent     = swapChain.getSwapChainExtent();
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
        // clearValues[0].depthStencil = ? ;
        clearValues[1].depthStencil    = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();
        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        anaPipeline->bind(commandBuffers[i]);
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void APP::drawFrame()
{
    uint32_t imageIndex;
    auto result = swapChain.acquireNextImage(&imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

} // namespace ana