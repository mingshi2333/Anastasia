#pragma once
#include "handles.h"
#include "rhi_def.h"
#include <string>
#include <functional>

namespace ana::rhi {

    class IDeviceContext;

    class RenderGraphPassBuilder {
    public:
        // e.g., createAttachment("g_albedo", format::rgba8);
        virtual void createAttachment(const std::string& name, Format format) = 0;
        // e.g., readAttachment("g_albedo");
        virtual void readAttachment(const std::string& name) = 0;
    };

    using RenderGraphPassExecuteFunc = std::function<void(IDeviceContext*)>;
    using RenderGraphPassSetupFunc = std::function<void(RenderGraphPassBuilder&)>;

    // Opaque interface
    class IRenderGraph {
    public:
        virtual ~IRenderGraph() = default;

        virtual void addGraphicsPass(const std::string& name, RenderGraphPassSetupFunc setup, RenderGraphPassExecuteFunc execute) = 0;
        
        virtual void compile() = 0;
        virtual void execute() = 0;
    };

} // namespace ana::rhi
