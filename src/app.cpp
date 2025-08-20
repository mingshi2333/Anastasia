#include "app.h"
#include "api/pipeline.h"
#include "api/vulkan/model.h"
#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
// #include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
// #include <vulkan/vulkan_handles.hpp>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace ana
{
APP::APP()
{
    loadModel();
    createPipelineLayout();
    recreateSwapChain();
    initImGui();
    createCommandBuffers();
}

APP::~APP()
{
    vkDeviceWaitIdle(device.device());
    shutdownImGui();
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void APP::run()
{
    while (!window.shouldClose())
    {
        glfwPollEvents();
        drawFrame();
    }
    vkDeviceWaitIdle(device.device());
}

void APP::initImGui()
{
    // 1: create descriptor pool for IMGUI
    //  the size of the pool is very oversize, but it's copied from official example and should be OK
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets                    = 1000;
    pool_info.poolSizeCount              = std::size(pool_sizes);
    pool_info.pPoolSizes                 = pool_sizes;

    if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create imgui descriptor pool");
    }

    // 2: initialize imgui library
    // this initializes the core structures of imgui
    ImGui::CreateContext();

    // this initializes imgui for GLFW
    ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);

    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance                  = device.getInstance();
    init_info.PhysicalDevice            = device.getPhysicalDevice();
    init_info.Device                    = device.device();
    init_info.Queue                     = device.graphicsQueue();
    init_info.DescriptorPool            = imguiPool;
    init_info.MinImageCount             = swapChain->imageCount();
    init_info.ImageCount                = swapChain->imageCount();
    init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
    init_info.RenderPass                = VK_NULL_HANDLE;
    init_info.UseDynamicRendering       = true;

    // Required by dynamic rendering
    static VkFormat color_format;
    color_format                                               = swapChain->getSwapChainImageFormat();
    init_info.PipelineRenderingCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &color_format;
    init_info.PipelineRenderingCreateInfo.depthAttachmentFormat   = swapChain->findDepthFormat();
    init_info.PipelineRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

    ImGui_ImplVulkan_Init(&init_info);
}

void APP::shutdownImGui()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(device.device(), imguiPool, nullptr);
}

void APP::renderImGui(VkCommandBuffer commandBuffer)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    // ImGui::ShowDemoWindow();
    ImGui::Render();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
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
    assert(swapChain != nullptr && "Cannot create pipeline before swap chain");
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
    ana::vk::PipelineConfigInfo pipelineConfig{};
    vk::ANAPipeline::defaultPipelineConfigInfo(pipelineConfig);
    //pipelineConfig.renderPass            = swapChain->getRenderPass();
    pipelineConfig.colorAttachmentFormat = swapChain->getSwapChainImageFormat();
    pipelineConfig.depthAttachmentFormat = swapChain->findDepthFormat();
    pipelineConfig.pipelineLayout        = pipelineLayout;
    anaPipeline =
        std::make_unique<vk::ANAPipeline>(device, "../shaders/vert.spv", "../shaders/frag.spv", pipelineConfig);
}

void APP::freeCommandBuffers()
{
    vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()),
                         commandBuffers.data());
    commandBuffers.clear();
}

void APP::recreateSwapChain()
{
    auto extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0)
    {
        extent = window.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.device());

    if (swapChain == nullptr)
    {
        swapChain = std::make_shared<vk::SwapChain>(device, extent);
    }
    else
    {
        swapChain = std::make_shared<vk::SwapChain>(device, extent, std::move(swapChain));
    }
    if (swapChain->imageCount() != commandBuffers.size())
    {
        freeCommandBuffers();
        createCommandBuffers();
    }
    createPipeline();
}

void APP::createCommandBuffers()
{

    // commnandBuffers.resize(swapChain->imageCount());

    commandBuffers.resize(swapChain->imageCount());
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool        = device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void APP::sierpinski(std::vector<Model::Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top)
{
    if (depth <= 0)
    {
        vertices.push_back({ top });
        vertices.push_back({ right });
        vertices.push_back({ left });
    }
    else
    {
        auto leftTop   = 0.5f * (left + top);
        auto rightTop  = 0.5f * (right + top);
        auto leftRight = 0.5f * (left + right);
        sierpinski(vertices, depth - 1, left, leftRight, leftTop);
        sierpinski(vertices, depth - 1, leftRight, right, rightTop);
        sierpinski(vertices, depth - 1, leftTop, rightTop, top);
    }
}

void APP::loadModel()
{
    std::vector<Model::Vertex> vertices{
        { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, 0.5f },  { 0.0f, 1.0f, 0.0f } },
        { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
    };
    // sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
    model = std::make_unique<ana::Model>(device, vertices);
}

void APP::drawFrame()
{
    uint32_t imageIndex;
    auto result = swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // Transition layout from UNDEFINED to COLOR_ATTACHMENT_OPTIMAL
    VkImageMemoryBarrier imageMemoryBarrier_to_color{};
    imageMemoryBarrier_to_color.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier_to_color.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier_to_color.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier_to_color.srcAccessMask                   = 0;
    imageMemoryBarrier_to_color.dstAccessMask                   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier_to_color.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier_to_color.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier_to_color.image                           = swapChain->getImage(imageIndex);
    imageMemoryBarrier_to_color.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier_to_color.subresourceRange.baseMipLevel   = 0;
    imageMemoryBarrier_to_color.subresourceRange.levelCount     = 1;
    imageMemoryBarrier_to_color.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier_to_color.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(commandBuffers[imageIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &imageMemoryBarrier_to_color);

    // Dynamic Rendering Begin
    VkRenderingAttachmentInfo colorAttachmentInfo{};
    colorAttachmentInfo.sType            = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachmentInfo.imageView        = swapChain->getImageView(imageIndex);
    colorAttachmentInfo.imageLayout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.clearValue.color = { 0.1f, 0.1f, 0.1f, 1.0f };

    VkRenderingAttachmentInfo depthAttachmentInfo{};
    depthAttachmentInfo.sType                   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachmentInfo.imageView               = swapChain->getDepthImageView(imageIndex);
    depthAttachmentInfo.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachmentInfo.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.clearValue.depthStencil = { 1.0f, 0 };

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset    = { 0, 0 };
    renderingInfo.renderArea.extent    = swapChain->getSwapChainExtent();
    renderingInfo.layerCount           = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments    = &colorAttachmentInfo;
    renderingInfo.pDepthAttachment     = &depthAttachmentInfo;
    renderingInfo.pStencilAttachment   = nullptr;

    vkCmdBeginRendering(commandBuffers[imageIndex], &renderingInfo);

    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(swapChain->getSwapChainExtent().width);
    viewport.height   = static_cast<float>(swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain->getSwapChainExtent();
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

    anaPipeline->bind(commandBuffers[imageIndex]);

    model->bind(commandBuffers[imageIndex]);
    model->draw(commandBuffers[imageIndex]);

    // vkCmdDraw(commandBuffers[imageIndex], 3, 1, 0, 0);

    renderImGui(commandBuffers[imageIndex]);

    vkCmdEndRendering(commandBuffers[imageIndex]);

    // Transition layout from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
    VkImageMemoryBarrier imageMemoryBarrier_to_present{};
    imageMemoryBarrier_to_present.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier_to_present.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier_to_present.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier_to_present.srcAccessMask                   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier_to_present.dstAccessMask                   = 0;
    imageMemoryBarrier_to_present.oldLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier_to_present.newLayout                       = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageMemoryBarrier_to_present.image                           = swapChain->getImage(imageIndex);
    imageMemoryBarrier_to_present.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier_to_present.subresourceRange.baseMipLevel   = 0;
    imageMemoryBarrier_to_present.subresourceRange.levelCount     = 1;
    imageMemoryBarrier_to_present.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier_to_present.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(commandBuffers[imageIndex], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &imageMemoryBarrier_to_present);

    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }

    result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
    {
        window.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

} // namespace ana
