#include "app.h"
#include "api/gameobject.h"
#include "api/pipeline.h"
#include "api/vulkan/device.h"
#include "api/vulkan/model.h"
#include "api/vulkan/renderer.h"
#include "glm/common.hpp"
#include "math/math.h"
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

#include "glm/fwd.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "camera/camera.h"

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

    Camera camera{ CameraType::Perspective };

    std::cout << "maxPushConstantSize= " << device.properties.limits.maxPushConstantsSize << std::endl;

    while (!window.shouldClose())
    {
        glfwPollEvents();

        float aspect = renderer.getAspectRatio();
        // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
        PerspectiveInfo perspectiveInfo{ aspect, 50, 0.1f, 10.f };
        camera.setProjection(perspectiveInfo);
        if (auto commandBuffer = renderer.beginFrame())
        {
            renderer.beginSwapChainRendererPass(commandBuffer);
            rendersystem.renderGameObjects(commandBuffer, gameObjects, camera);
            renderer.endSwapChainRendererPass(commandBuffer);
            renderer.endFrame();
        }
    }
    // vkDeviceWaitIdle(device.device());
}

std::unique_ptr<ana::Model> createCubeModel(vk::Device& device, glm::vec3 offset)
{
    std::vector<ana::Model::Vertex> vertices{

        // left face (white)
        { { -.5f, -.5f, -.5f },  { .9f, .9f, .9f } },
        { { -.5f, .5f, .5f },    { .9f, .9f, .9f } },
        { { -.5f, -.5f, .5f },   { .9f, .9f, .9f } },
        { { -.5f, -.5f, -.5f },  { .9f, .9f, .9f } },
        { { -.5f, .5f, -.5f },   { .9f, .9f, .9f } },
        { { -.5f, .5f, .5f },    { .9f, .9f, .9f } },

        // right face (yellow)
        { { .5f, -.5f, -.5f },   { .8f, .8f, .1f } },
        { { .5f, .5f, .5f },     { .8f, .8f, .1f } },
        { { .5f, -.5f, .5f },    { .8f, .8f, .1f } },
        { { .5f, -.5f, -.5f },   { .8f, .8f, .1f } },
        { { .5f, .5f, -.5f },    { .8f, .8f, .1f } },
        { { .5f, .5f, .5f },     { .8f, .8f, .1f } },

        // top face (orange, remember y axis points down)
        { { -.5f, -.5f, -.5f },  { .9f, .6f, .1f } },
        { { .5f, -.5f, .5f },    { .9f, .6f, .1f } },
        { { -.5f, -.5f, .5f },   { .9f, .6f, .1f } },
        { { -.5f, -.5f, -.5f },  { .9f, .6f, .1f } },
        { { .5f, -.5f, -.5f },   { .9f, .6f, .1f } },
        { { .5f, -.5f, .5f },    { .9f, .6f, .1f } },

        // bottom face (red)
        { { -.5f, .5f, -.5f },   { .8f, .1f, .1f } },
        { { .5f, .5f, .5f },     { .8f, .1f, .1f } },
        { { -.5f, .5f, .5f },    { .8f, .1f, .1f } },
        { { -.5f, .5f, -.5f },   { .8f, .1f, .1f } },
        { { .5f, .5f, -.5f },    { .8f, .1f, .1f } },
        { { .5f, .5f, .5f },     { .8f, .1f, .1f } },

        // nose face (blue)
        { { -.5f, -.5f, 0.5f },  { .1f, .1f, .8f } },
        { { .5f, .5f, 0.5f },    { .1f, .1f, .8f } },
        { { -.5f, .5f, 0.5f },   { .1f, .1f, .8f } },
        { { -.5f, -.5f, 0.5f },  { .1f, .1f, .8f } },
        { { .5f, -.5f, 0.5f },   { .1f, .1f, .8f } },
        { { .5f, .5f, 0.5f },    { .1f, .1f, .8f } },

        // tail face (green)
        { { -.5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
        { { .5f, .5f, -0.5f },   { .1f, .8f, .1f } },
        { { -.5f, .5f, -0.5f },  { .1f, .8f, .1f } },
        { { -.5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
        { { .5f, -.5f, -0.5f },  { .1f, .8f, .1f } },
        { { .5f, .5f, -0.5f },   { .1f, .8f, .1f } },
    };
    for (auto& v : vertices)
    {
        v.position += offset;
    }
    return std::make_unique<ana::Model>(device, vertices);
}

void APP::loadGameObjects()
{
    std::shared_ptr<ana::Model> Model = createCubeModel(device, { .0f, .0f, .0f });
    auto cube                         = GameObject::createGameObject();
    cube.model                        = Model;
    cube.transform.translation        = { .0f, .0f, 2.5f };
    cube.transform.scale              = { .5f, .5f, .5f };
    gameObjects.push_back(std::move(cube));
}

} // namespace ana
