#include "app.h"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#define BACKWARD_HAS_DW 1
#include "backward.hpp"

namespace backward
{
backward::SignalHandling sh;

}

int main()
{
    ana::APP app{};

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}