#pragma once
#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ANA
{
class ANA_window
{
public:
    ANA_window(int width, int height, std::string windowName);
    ~ANA_window();
    ANA_window(const ANA_window&) = delete;
    ANA_window& operator=(const ANA_window&) = delete;
    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    void cleanup();
    bool shouldClose()
    {
        return glfwWindowShouldClose(window);
    }

private:
    void initWindow();
    GLFWwindow* window;
    int width;
    int height;
    std::string windowName;
};

} // namespace ANA