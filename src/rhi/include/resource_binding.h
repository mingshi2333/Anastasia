#pragma once
#include "handles.h"
#include <string>

namespace ana::rhi {

    class IShaderVariable {
    public:
        virtual ~IShaderVariable() = default;
        virtual void set(TextureViewHandle view) = 0;
        virtual void set(BufferViewHandle view) = 0;
        virtual void set(SamplerHandle sampler) = 0;
    };

    // Opaque interface, implementation will be in the backend.
    class IShaderResourceBinding {
    public:
        virtual ~IShaderResourceBinding() = default;
        virtual IShaderVariable* getVariableByName(const std::string& name) = 0;
    };

} // namespace ana::rhi
