#pragma once
#include "Pipeline.h"
#include "VertexArray.h"
#include "Framebuffer.h"

namespace Eklipse
{
    class EK_API GraphicsAPI
    {
    public:
        static Unique<GraphicsAPI> Create();
        GraphicsAPI();

        virtual bool Init() = 0;
        virtual void Shutdown() = 0;
        virtual void WaitDeviceIdle() = 0;

        virtual void BeginFrame() = 0;
        virtual void Submit() = 0;

        virtual void BeginDefaultRenderPass() = 0;
        virtual void EndDefaultRenderPass() = 0;

        virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;

        virtual void DrawIndexed(Ref<VertexArray> vertexArray) = 0;

    protected:
        bool m_initialized = false;

        // TODO: more like... render pipeline (raster / raytracing)
        //Pipeline m_pipeline;
    };
}