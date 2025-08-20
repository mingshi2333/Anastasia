#pragma once

#include "vulkan/device.h"
#include "vulkan/model.h"
#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ana::vk
{

struct PipelineConfigInfo
{
    PipelineConfigInfo()                                     = default;
    PipelineConfigInfo(const PipelineConfigInfo&)            = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    // VkViewport viewport{};
    // VkRect2D scissor{};
    VkPipelineViewportStateCreateInfo viewportInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    std::vector<VkDynamicState> dynamicStateEnables{};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    VkPipelineLayout pipelineLayout{};
    VkRenderPass renderPass = nullptr;
    uint32_t subpass        = 0;

    VkFormat colorAttachmentFormat;
    VkFormat depthAttachmentFormat;
};

class ANAPipeline
{
public:
    ANAPipeline(Device& Anadevice, const std::string& vertFilepath, const std::string& fragFilepath,
                const PipelineConfigInfo& configInfo);
    ~ANAPipeline();
    ANAPipeline(const ANAPipeline&)            = delete;
    ANAPipeline& operator=(const ANAPipeline&) = delete;
    ANAPipeline(ANAPipeline&&)                 = delete;
    ANAPipeline& operator=(ANAPipeline&&)      = delete;

    void bind(VkCommandBuffer commandBuffer);

    static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

private:
    static std::vector<char> readFile(const std::string& filename);
    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath,
                                const PipelineConfigInfo& configInfo);

    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    Device& Anadevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};
} // namespace ana::vk