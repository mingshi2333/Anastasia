#pragma once
#include "handles.h"
#include "rhi_def.h"

namespace ana::rhi {

// Descriptors for creating resources
// These will be filled out with more detail later.

struct BufferDesc {
    uint64_t size = 0;
    // ... flags for usage (vertex, index, uniform), memory properties, etc.
};

struct TextureDesc {
    uint32_t width = 0;
    uint32_t height = 0;
    Format format = Format::UNDEFINED;
    // ... flags for usage, mip levels, etc.
};

struct SamplerDesc {
    // ... filter, address mode, etc.
};

struct TextureViewDesc {
    TextureHandle texture;
    Format format = Format::UNDEFINED;
    // ... mip level, array layer, etc.
};

// Opaque interfaces for resources and views, if needed for type safety.
// For a pure handle-based system, these might not be strictly necessary
// but can be useful for polymorphism in the backend.
class IResource {};
class IBuffer : public IResource {};
class ITexture : public IResource {};
class ISampler : public IResource {};

class IResourceView {};
class IBufferView : public IResourceView {};
class ITextureView : public IResourceView {};


} // namespace ana::rhi
