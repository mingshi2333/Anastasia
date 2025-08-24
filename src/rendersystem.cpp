#include "api/gameobject.h"
#include "api/pipeline.h"
#include "api/vulkan/device.h"
#include "glm/fwd.hpp"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "rendersystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace ana
{

struct SimplePushConstantData
{
    glm::mat2 transform{ 1.f };
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

RenderSystem::RenderSystem(vk::Device& device, VkFormat colorFormat, VkFormat depthFormat)
    : device(device)
{

    createPipelineLayout();
    createPipeline(colorFormat, depthFormat);
}

RenderSystem::~RenderSystem()
{
    vkDeviceWaitIdle(device.device());
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void RenderSystem::createPipelineLayout()
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset     = 0;
    pushConstantRange.size       = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount         = 0;
    pipelineLayoutInfo.pSetLayouts            = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;
    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void RenderSystem::createPipeline(VkFormat colorFormat, VkFormat depthFormat)
{
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
    ana::vk::PipelineConfigInfo pipelineConfig{};
    vk::ANAPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.pipelineLayout        = pipelineLayout;
    pipelineConfig.colorAttachmentFormat = colorFormat;
    pipelineConfig.depthAttachmentFormat = depthFormat;
    anaPipeline = std::make_unique<vk::ANAPipeline>(device, "shaders/vert.spv", "shaders/frag.spv", pipelineConfig);
}

void RenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects)
{
    anaPipeline->bind(commandBuffer);
    for (auto& obj : gameObjects)
    {
        obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());
        SimplePushConstantData push{};
        push.offset    = obj.transform2d.translation;
        push.color     = obj.color;
        push.transform = obj.transform2d.mat2();

        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(SimplePushConstantData), &push);

        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}

} // namespace ana
