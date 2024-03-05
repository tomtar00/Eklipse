#pragma once
#include "Pipeline.h"
#include "VertexArray.h"
#include "Framebuffer.h"

namespace Eklipse
{
    class EK_API GraphicsAPI
    {
    public:
        enum class Type
        {
            Vulkan = 0,
            OpenGL = 1
        };
        static const uint32_t TYPE_COUNT = 2;
        static const String TypeToString(Type type);

    public:
        GraphicsAPI();
        static Unique<GraphicsAPI> Create();

        virtual bool Init() = 0;
        virtual void Shutdown() = 0;
        virtual void WaitDeviceIdle() = 0;

        virtual void BeginFrame() = 0;
        virtual void BeginComputePass() = 0;
        virtual void EndComputePass() = 0;
        virtual void Submit() = 0;

        virtual void SetPipelineTopologyMode(Pipeline::TopologyMode topologyMode) = 0;
        virtual void SetPipelineType(Pipeline::Type type) = 0;

        virtual void DrawIndexed(Ref<VertexArray> vertexArray) = 0;

    protected:
        bool m_initialized = false;
    };
}