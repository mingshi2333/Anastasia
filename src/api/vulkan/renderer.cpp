#include "renderer.h"
#include "swapchain.h"
#include <stdexcept>

namespace ana
{

struct SimplePushConstantData
{
    glm::mat2 transform{ 1.f };
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

Renderer::Renderer(ANAwindow& window, vk::Device& device)
    : window{ window }
    , device{ device }
{
    recreateSwapChain();
    // Allocate per-frame-in-flight command buffers once
    createCommandBuffers();
    // initImGui();
}

Renderer::~Renderer()
{
    freeCommandBuffers();
}

void Renderer::freeCommandBuffers()
{
    if (!commandBuffers.empty())
    {
        vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()),
                             commandBuffers.data());
        commandBuffers.clear();
    }
}

void Renderer::recreateSwapChain()
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
        std::shared_ptr<vk::SwapChain> oldSwapChain = std::move(swapChain);
        swapChain                                   = std::make_shared<vk::SwapChain>(device, extent, oldSwapChain);

        if (!oldSwapChain->compareSwapFormats(*swapChain.get()))
        {
            throw std::runtime_error("Swap chain image(or depth) format has changed!");

            //TODO should set a callback to let app deal with this
        }
    }

    //not depend on image count
    // if (swapChain->imageCount() != commandBuffers.size())
    // {
    //     freeCommandBuffers();
    //     createCommandBuffers();
    // }
}

void Renderer::createCommandBuffers()
{
    commandBuffers.resize(vk::SwapChain::MAX_FRAMES_IN_FLIGHT);
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

VkCommandBuffer Renderer::beginFrame()
{
    assert(!isFrameStarted && "can't call beginFrame while already in progress");
    auto result = swapChain->acquireNextImage(&currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    isFrameStarted     = true;
    auto commandBuffer = getCurrentCommandBuffer();
    vkResetCommandBuffer(commandBuffer, 0);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
}

void Renderer::endFrame()
{
    assert(isFrameStarted && "can't call endFrame while frame is not in progress");
    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
    auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
    {
        window.resetWindowResizedFlag();
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
    isFrameStarted    = false;
    currentFrameIndex = (currentFrameIndex + 1) % vk::SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRendererPass(VkCommandBuffer commandBuffer)
{
    assert(isFrameStarted && "cant't call beginSwapChainRendererPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() &&
           "Cant't beging Renderer pass on command buffer from a different frame");
    VkImageMemoryBarrier imageMemoryBarrier_to_color{};
    imageMemoryBarrier_to_color.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier_to_color.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier_to_color.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier_to_color.srcAccessMask                   = 0;
    imageMemoryBarrier_to_color.dstAccessMask                   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier_to_color.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier_to_color.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier_to_color.image                           = swapChain->getImage(currentImageIndex);
    imageMemoryBarrier_to_color.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier_to_color.subresourceRange.baseMipLevel   = 0;
    imageMemoryBarrier_to_color.subresourceRange.levelCount     = 1;
    imageMemoryBarrier_to_color.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier_to_color.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(getCurrentCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &imageMemoryBarrier_to_color);

    // Dynamic Renderering Begin
    VkRenderingAttachmentInfo colorAttachmentInfo{};
    colorAttachmentInfo.sType            = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachmentInfo.imageView        = swapChain->getImageView(currentImageIndex);
    colorAttachmentInfo.imageLayout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.clearValue.color = { 0.01f, 0.01f, 0.01f, 1.0f };

    VkRenderingAttachmentInfo depthAttachmentInfo{};
    depthAttachmentInfo.sType                   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachmentInfo.imageView               = swapChain->getDepthImageView(currentImageIndex);
    depthAttachmentInfo.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachmentInfo.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.clearValue.depthStencil = { 1.0f, 0 };

    VkRenderingInfo RendereringInfo{};
    RendereringInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
    RendereringInfo.renderArea.offset    = { 0, 0 };
    RendereringInfo.renderArea.extent    = swapChain->getSwapChainExtent();
    RendereringInfo.layerCount           = 1;
    RendereringInfo.colorAttachmentCount = 1;
    RendereringInfo.pColorAttachments    = &colorAttachmentInfo;
    RendereringInfo.pDepthAttachment     = &depthAttachmentInfo;
    RendereringInfo.pStencilAttachment   = nullptr;

    vkCmdBeginRendering(commandBuffer, &RendereringInfo);

    VkViewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(swapChain->getSwapChainExtent().width);
    viewport.height   = static_cast<float>(swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain->getSwapChainExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endSwapChainRendererPass(VkCommandBuffer commandBuffer)
{
    assert(isFrameStarted && "cant't call endSwapChainRendererPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() &&
           "Cant't ending Renderer pass on command buffer from a different frame");
    vkCmdEndRendering(commandBuffer);
    // Transition layout from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
    VkImageMemoryBarrier imageMemoryBarrier_to_present{};
    imageMemoryBarrier_to_present.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier_to_present.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier_to_present.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier_to_present.srcAccessMask                   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier_to_present.dstAccessMask                   = 0;
    imageMemoryBarrier_to_present.oldLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier_to_present.newLayout                       = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageMemoryBarrier_to_present.image                           = swapChain->getImage(currentImageIndex);
    imageMemoryBarrier_to_present.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier_to_present.subresourceRange.baseMipLevel   = 0;
    imageMemoryBarrier_to_present.subresourceRange.levelCount     = 1;
    imageMemoryBarrier_to_present.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier_to_present.subresourceRange.layerCount     = 1;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1,
                         &imageMemoryBarrier_to_present);
}

} // namespace ana
