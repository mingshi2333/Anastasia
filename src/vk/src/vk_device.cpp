#include "vk_device.h"

// For now, the implementation is just a stub.
// We will fill this in later.

namespace ana::vk {

Device::Device() {
    // TODO: Initialize Vulkan Instance, Device, Queues, VMA, etc.
}

Device::~Device() {
    // TODO: Destroy all Vulkan resources.
}

rhi::PipelineStateHandle Device::createGraphicsPipeline(const rhi::GraphicsPipelineDesc& desc) {
    // TODO: Implement pipeline creation.
    return {};
}

rhi::IDeviceContext* Device::getImmediateContext() {
    // TODO: Return the immediate context.
    return nullptr;
}

} // namespace ana::vk
