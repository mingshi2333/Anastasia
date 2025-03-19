#pragma once
#include <string>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ANA
{
class ANAwindow
{
public:
    ANAwindow(int width, int height, std::string windowName);
    ~ANAwindow();
    ANAwindow(const ANAwindow&) = delete;
    ANAwindow& operator=(const ANAwindow&) = delete;
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