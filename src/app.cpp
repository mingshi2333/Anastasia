#include "app.h"
#include "api/gameobject.h"
#include "api/pipeline.h"
#include "api/vulkan/model.h"
#include "api/vulkan/renderer.h"
#include "glm/common.hpp"
#include "rendersystem.h"
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

APP::APP()
{
    loadGameObjects();
}

APP::~APP()
{
}

void APP::run()
{
    RenderSystem rendersystem{ device, renderer.getSwapChainImageFormat(), renderer.getSwapChainDepthFormat() };
    std::cout << "maxPushConstantSize= " << device.properties.limits.maxPushConstantsSize << std::endl;

    while (!window.shouldClose())
    {
        glfwPollEvents();
        if (auto commandBuffer = renderer.beginFrame())
        {
            renderer.beginSwapChainRendererPass(commandBuffer);
            rendersystem.renderGameObjects(commandBuffer, gameObjects);
            renderer.endSwapChainRendererPass(commandBuffer);
            renderer.endFrame();
        }
    }
    // vkDeviceWaitIdle(device.device());
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

} // namespace ana
