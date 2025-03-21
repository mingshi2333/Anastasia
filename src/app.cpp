#include "app.h"

namespace ana
{
void APP::run()
{
    while (!window.shouldClose())
    {
        glfwPollEvents();
    }
}
} // namespace ana