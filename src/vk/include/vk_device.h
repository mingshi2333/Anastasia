#pragma once
#include "device.h"

namespace ana::vk {

    class Device final : public rhi::IRenderDevice {
    public:
        Device();
        ~Device() override;

        rhi::PipelineStateHandle createGraphicsPipeline(const rhi::GraphicsPipelineDesc& desc) override;

        rhi::IDeviceContext* getImmediateContext() override;
    };

} // namespace ana::vk
