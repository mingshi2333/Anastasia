#pragma once

#include "vulkan/device.h"
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace ana::vk
{

struct PipelineConfigInfo
{
    VkViewport viewport{};
    VkRect2D scissor{};
    // VkPipelineViewportStateCreateInfo viewportInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    VkPipelineLayout pipelineLayout{};
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

    static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

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