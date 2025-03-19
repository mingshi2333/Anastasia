#pragma once

#include "ANA_window.h"
#include "ANA_pipeline.h"
namespace ANA
{
class APP
{
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();

private:
    ANAwindow window{WIDTH, HEIGHT, "Vulkan"};
    ANAPipeline pipeline{"../shaders/vert.spv", "../shaders/frag.spv"};
};
} // namespace ANA