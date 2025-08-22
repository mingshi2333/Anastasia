#include "app.h"
#include "api/gameobject.h"
#include "api/pipeline.h"
#include "api/vulkan/model.h"
#include "api/vulkan/renderer.h"
#include "glm/common.hpp"
#include <array>
#include <cstdint>
#include <iostream>
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

#include "glm/fwd.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
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

APP::APP()
{
    // loadModel();
    loadGameObjects();
    createPipelineLayout();
    // initImGui();
    createPipeline();
}

APP::~APP()
{
    vkDeviceWaitIdle(device.device());
    // shutdownImGui();
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void APP::run()
{
    std::cout << "maxPushConstantSize= " << device.properties.limits.maxPushConstantsSize << std::endl;

    while (!window.shouldClose())
    {
        glfwPollEvents();
        // drawFrame();
        if (auto commandBuffer = renderer.beginFrame())
        {
            renderer.beginSwapChainRendererPass(commandBuffer);
            renderGameObject(commandBuffer);
            renderer.endSwapChainRendererPass(commandBuffer);
            renderer.endFrame();
        }
    }
    vkDeviceWaitIdle(device.device());
}

// void APP::initImGui()
// {
//     // 1: create descriptor pool for IMGUI
//     //  the size of the pool is very oversize, but it's copied from official example and should be OK
//     VkDescriptorPoolSize pool_sizes[] = {
//         { VK_DESCRIPTOR_TYPE_SAMPLER,                1000 },
//         { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
//         { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000 },
//         { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000 },
//         { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000 },
//         { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000 },
//         { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000 },
//         { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000 },
//         { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
//         { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
//         { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000 }
//     };

//     VkDescriptorPoolCreateInfo pool_info = {};
//     pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//     pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//     pool_info.maxSets                    = 1000;
//     pool_info.poolSizeCount              = std::size(pool_sizes);
//     pool_info.pPoolSizes                 = pool_sizes;

//     if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &imguiPool) != VK_SUCCESS)
//     {
//         throw std::runtime_error("failed to create imgui descriptor pool");
//     }

//     // 2: initialize imgui library
//     // this initializes the core structures of imgui
//     ImGui::CreateContext();

//     // this initializes imgui for GLFW
//     ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);

//     // this initializes imgui for Vulkan
//     ImGui_ImplVulkan_InitInfo init_info = {};
//     init_info.Instance                  = device.getInstance();
//     init_info.PhysicalDevice            = device.getPhysicalDevice();
//     init_info.Device                    = device.device();
//     init_info.Queue                     = device.graphicsQueue();
//     init_info.DescriptorPool            = imguiPool;
//     init_info.MinImageCount             = swapChain->imageCount();
//     init_info.ImageCount                = swapChain->imageCount();
//     init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
//     init_info.RenderPass                = VK_NULL_HANDLE;
//     init_info.UseDynamicRendering       = true;

//     // Required by dynamic rendering
//     static VkFormat color_format;
//     color_format                                               = swapChain->getSwapChainImageFormat();
//     init_info.PipelineRenderingCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
//     init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
//     init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &color_format;
//     init_info.PipelineRenderingCreateInfo.depthAttachmentFormat   = swapChain->findDepthFormat();
//     init_info.PipelineRenderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

//     ImGui_ImplVulkan_Init(&init_info);
// }

// void APP::shutdownImGui()
// {
//     ImGui_ImplVulkan_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();
//     vkDestroyDescriptorPool(device.device(), imguiPool, nullptr);
// }

// void APP::renderImGui(VkCommandBuffer commandBuffer)
// {
//     ImGui_ImplVulkan_NewFrame();
//     ImGui_ImplGlfw_NewFrame();

//     ImGui::NewFrame();
//     // ImGui::ShowDemoWindow();
//     ImGui::Render();

//     ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
// }

void APP::createPipelineLayout()
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

void APP::createPipeline()
{
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
    ana::vk::PipelineConfigInfo pipelineConfig{};
    vk::ANAPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass            = renderer.getSwapChainRendererPass();
    pipelineConfig.colorAttachmentFormat = renderer.getSwapChainImageFormat();
    pipelineConfig.depthAttachmentFormat = renderer.getSwapChainDepthFormat();
    pipelineConfig.pipelineLayout        = pipelineLayout;
    anaPipeline =
        std::make_unique<vk::ANAPipeline>(device, "../shaders/vert.spv", "../shaders/frag.spv", pipelineConfig);
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

void APP::loadGameObjects()
{
    std::vector<Model::Vertex> vertices{
        { { 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, 0.5f },  { 0.0f, 1.0f, 0.0f } },
        { { -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }
    };
    // sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
    auto model     = std::make_shared<ana::Model>(device, vertices);
    auto triangle  = GameObject::createGameObject();
    triangle.model = model;
    triangle.color = { .1f, .8f, .1f };
    // triangle.transform2d.translation.x = .2f;
    triangle.transform2d.scale    = { 2.f, .5f };
    triangle.transform2d.rotation = .25f * glm::two_pi<float>();
    gameObjects.push_back(std::move(triangle));
}

void APP::renderGameObject(VkCommandBuffer commandBuffer)
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
