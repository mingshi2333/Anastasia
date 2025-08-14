#pragma once
#include "handles.h"
#include "rhi_def.h"
#include <string>
#include <vector>

namespace ana::rhi
{

struct ShaderDesc
{
    std::string path;
    // TODO: Add entry point, shader stage, etc.
};

struct GraphicsPipelineDesc
{
    std::vector<ShaderDesc> shaders;
    // TODO: Add RasterizerState, BlendState, DepthStencilState, etc.
};

// Opaque interface, implementation will be in the backend.
class IPipelineState
{
public:
    virtual ~IPipelineState()                                         = default;
    virtual ShaderResourceBindingHandle createShaderResourceBinding() = 0;
};

} // namespace ana::rhi
