#pragma once
#include <cstdint>
#include <functional>

namespace ana::rhi
{

// Use a simple ID-based handle system.
// The 0 ID is considered invalid/null.
#define DEFINE_RHI_HANDLE(Name)                  \
    struct Name                                  \
    {                                            \
        uint64_t id = 0;                         \
        bool operator==(const Name& other) const \
        {                                        \
            return id == other.id;               \
        }                                        \
    };

DEFINE_RHI_HANDLE(BufferHandle)
DEFINE_RHI_HANDLE(BufferViewHandle)
DEFINE_RHI_HANDLE(TextureHandle)
DEFINE_RHI_HANDLE(TextureViewHandle)
DEFINE_RHI_HANDLE(SamplerHandle)
DEFINE_RHI_HANDLE(PipelineStateHandle)
DEFINE_RHI_HANDLE(ShaderResourceBindingHandle)

} // namespace ana::rhi

// Add a hash for the handle so it can be used in unordered_maps
namespace std
{
template <>
struct hash<ana::rhi::BufferHandle>
{
    size_t operator()(const ana::rhi::BufferHandle& h) const
    {
        return hash<uint64_t>()(h.id);
    }
};

template <>
struct hash<ana::rhi::TextureHandle>
{
    size_t operator()(const ana::rhi::TextureHandle& h) const
    {
        return hash<uint64_t>()(h.id);
    }
};

template <>
struct hash<ana::rhi::TextureViewHandle>
{
    size_t operator()(const ana::rhi::TextureViewHandle& h) const
    {
        return hash<uint64_t>()(h.id);
    }
};

template <>
struct hash<ana::rhi::SamplerHandle>
{
    size_t operator()(const ana::rhi::SamplerHandle& h) const
    {
        return hash<uint64_t>()(h.id);
    }
};

template <>
struct hash<ana::rhi::PipelineStateHandle>
{
    size_t operator()(const ana::rhi::PipelineStateHandle& h) const
    {
        return hash<uint64_t>()(h.id);
    }
};

template <>
struct hash<ana::rhi::ShaderResourceBindingHandle>
{
    size_t operator()(const ana::rhi::ShaderResourceBindingHandle& h) const
    {
        return hash<uint64_t>()(h.id);
    }
};
} // namespace std
