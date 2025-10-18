#pragma once

#include "api/gameobject.h"
#include "api/pipeline.h"
#include "api/vulkan/device.h"
#include "camera/camera.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ana
{
class RenderSystem
{
public:
    RenderSystem(vk::Device& device, VkFormat colorFormat, VkFormat depthFormat);
    ~RenderSystem();
    RenderSystem(const RenderSystem&)            = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;
    RenderSystem(RenderSystem&&)                 = delete;
    RenderSystem& operator=(RenderSystem&&)      = delete;
    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, Camera& cmaera);

private:
    void createPipelineLayout();
    void createPipeline(VkFormat colorFormat, VkFormat depthFormat);

    vk::Device& device;
    std::unique_ptr<vk::ANAPipeline> anaPipeline;
    VkPipelineLayout pipelineLayout;
};
} // namespace ana
