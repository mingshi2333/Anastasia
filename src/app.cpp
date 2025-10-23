#include "app.h"
#include "api/gameobject.h"
#include "api/pipeline.h"
#include "api/vulkan/device.h"
#include "api/vulkan/model.h"
#include "api/vulkan/renderer.h"
#include "event/event.h"
#include "glm/common.hpp"
#include "math/math.h"
#include "rendersystem.h"
#include "wsi/wsi.h"
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

// #define VMA_IMPLEMENTATION
// #include <vk_mem_alloc.h>

#include "glm/fwd.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "camera/camera.h"

#include "event/eventManager.h"
#include <chrono>

namespace ana
{

APP::APP()
{
}

APP::~APP()
{
}

void APP::run()
{
    wsi          = ana::wsi::CreateGLFWWSI(WIDTH, HEIGHT, "Anastasia");
    device       = std::make_unique<vk::Device>(*wsi);
    renderer     = std::make_unique<Renderer>(*wsi, *device);
    renderSystem = std::make_unique<RenderSystem>(*device, renderer->getSwapChainImageFormat(),
                                                  renderer->getSwapChainDepthFormat());

    loadGameObjects();

    ana::EventManager em{};
    bool kW = false, kA = false, kS = false, kD = false;
    bool kShift = false, kUp = false, kDown = false;

    em.registerEvent<KeyboardEvent>(
        [&](const KeyboardEvent& e)
        {
            const bool pressed = (e.m_keystate == ana::KeyState::Pressed || e.m_keystate == ana::KeyState::Repeat);
            switch (e.m_key)
            {
            case ana::Key::W:
                kW = pressed;
                break;
            case ana::Key::A:
                kA = pressed;
                break;
            case ana::Key::S:
                kS = pressed;
                break;
            case ana::Key::D:
                kD = pressed;
                break;
            case ana::Key::LeftShift:
                kShift = pressed;
                break;
            case ana::Key::E:
                kUp = pressed;
                break;
            case ana::Key::Q:
                kDown = pressed;
                break;
            default:
                break;
            }
            return false;
        });

    if (wsi)
    {
        wsi->setKeySink(
            [&](const KeyboardEvent& e)
            {
                em.pushEvent<KeyboardEvent>(e);
            });
        wsi->setResizeSink(
            [&](const ana::WindowResizeEvent&)
            {
                if (renderer)
                {
                    renderer->notifyWindowResized();
                }
            });
    }

    Camera camera{ CameraType::Perspective };
    ana::Vec3 eye{ 0.0f, 0.0f, -0.2f };
    camera.setLookAt(eye, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });

    std::cout << "maxPushConstantSize= " << device->properties.limits.maxPushConstantsSize << std::endl;

    auto currentTime = std::chrono::high_resolution_clock::now();
    while (wsi && wsi->poll())
    {
        em.processAll();

        auto newTime    = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
        currentTime     = newTime;

        float speed = 2.0f * (kShift ? 3.0f : 1.0f);
        if (kW)
            eye.z += speed * frameTime;
        if (kS)
            eye.z -= speed * frameTime;
        if (kA)
            eye.x -= speed * frameTime;
        if (kD)
            eye.x += speed * frameTime;
        if (kUp)
            eye.y += speed * frameTime;
        if (kDown)
            eye.y -= speed * frameTime;
        camera.setLookAt(eye, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });

        float aspect = renderer->getAspectRatio();
        PerspectiveInfo perspectiveInfo{ aspect, 50.f, 0.1f, 10.f };
        camera.setProjection(perspectiveInfo);

        if (auto commandBuffer = renderer->beginFrame())
        {
            renderer->beginSwapChainRendererPass(commandBuffer);
            renderSystem->renderGameObjects(commandBuffer, gameObjects, camera);
            renderer->endSwapChainRendererPass(commandBuffer);
            renderer->endFrame();
        }
    }

    if (device)
    {
        vkDeviceWaitIdle(device->device());
    }
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
    std::shared_ptr<ana::Model> Model = createCubeModel(*device, { .0f, .0f, .0f });
    auto cube                         = GameObject::createGameObject();
    cube.model                        = Model;
    cube.transform.translation        = { .0f, .0f, 2.5f };
    cube.transform.scale              = { .5f, .5f, .5f };
    gameObjects.push_back(std::move(cube));
}

} // namespace ana
