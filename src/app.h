#pragma once

#include "ANA_window.h"

namespace ANA
{
class APP
{
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();

private:
    ANA_window window{WIDTH, HEIGHT, "Vulkan"};
};
} // namespace ANA