#include "app.h"
#include "ANA_window.h"

namespace ANA
{
void APP::run()
{
    while (!window.shouldClose())
    {
        glfwPollEvents();
    }
}
} // namespace ANA