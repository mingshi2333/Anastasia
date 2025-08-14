#pragma once
#include "handles.h"
#include "pipeline_state.h"

namespace ana::rhi
{

class IDeviceContext;

// Interface for the Render Device, responsible for resource creation.
class IRenderDevice
{
public:
    virtual ~IRenderDevice() = default;

    virtual PipelineStateHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc) = 0;
    // TODO: Add other resource creation methods, e.g.:
    // virtual BufferHandle createBuffer(const BufferDesc& desc) = 0;
    // virtual TextureHandle createTexture(const TextureDesc& desc) = 0;

    virtual IDeviceContext* getImmediateContext() = 0;
};

} // namespace ana::rhi
