#pragma once

#include "api/vulkan/ANA_window.h"
#include "api/pipeline.h"
namespace ana
{
class APP
{
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();

private:
    ANAwindow window{WIDTH, HEIGHT, "Vulkan"};
    vk::Device device{window};

    VkPipelineLayout pipelineLayout{};
    vk::ANAPipeline pipeline{
        device,
        "../shaders/vert.spv",
        "../shaders/frag.spv",
        vk::ANAPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT),
    };
};
} // namespace ana