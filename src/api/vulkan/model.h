#pragma once

#include "device.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace ana
{

class Model
{

public:
    struct Vertex
    {
        glm::vec2 position;

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttribuuteDescriptions();
    };

    Model(vk::Device& device, const std::vector<Vertex>& vertices);
    ~Model();

    Model(const Model&)            = delete;
    Model& operator=(const Model&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    void createVertexBuffers(const std::vector<Vertex>& vertices);

    vk::Device& device;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;
};

}; // namespace ana