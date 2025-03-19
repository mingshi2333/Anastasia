#include "ANA_window.h"

namespace ANA
{

ANA_window::ANA_window(int w, int h, std::string name) : width(w), height(h), windowName(name)
{
    initWindow();
}

ANA_window::~ANA_window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void ANA_window::initWindow()
{
    // glfw start
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}
} // namespace ANA