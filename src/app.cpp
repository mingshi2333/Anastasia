#include "app.h"
#include <vulkan/vulkan_core.h>
#include <stdexcept>

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
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}
void APP::run()
{
    while (!window.shouldClose())
    {
        glfwPollEvents();
    }
}

void APP::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void APP::createPipeline()
{
    auto pipelineConfig = vk::ANAPipeline::defaultPipelineConfigInfo(swapChain.width(), swapChain.height());
    pipelineConfig.renderPass = swapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    anaPipeline = std::make_unique<vk::ANAPipeline>(device, "../shaders/vert.spv", "../shaders/frag.spv", pipelineConfig);
}
void APP::createCommandBuffers()
{}

void APP::drawFrame()
{}

} // namespace ana