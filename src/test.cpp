#include "app.h"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

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