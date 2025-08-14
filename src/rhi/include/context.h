#pragma once
#include "handles.h"

namespace ana::rhi {

    class IRenderGraph;

    // Interface for the Device Context, responsible for command recording.
    class IDeviceContext {
    public:
        virtual ~IDeviceContext() = default;

        virtual void setPipeline(PipelineStateHandle pso) = 0;
        virtual void commitShaderResources(ShaderResourceBindingHandle srb) = 0;
        virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
        // ... other drawing commands

        virtual void transitionResourceState() = 0; // Simplified for now

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;

        virtual IRenderGraph& getRenderGraph() = 0;
    };

} // namespace ana::rhi
