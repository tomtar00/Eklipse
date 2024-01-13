#pragma once
#include "Pipeline.h"
#include "VertexArray.h"
#include "Framebuffer.h"

namespace Eklipse
{
	class GraphicsAPI
	{
	public:
		static Unique<GraphicsAPI> Create();
		GraphicsAPI();

		virtual bool Init() = 0;
		virtual void Shutdown() = 0;
		virtual void WaitDeviceIdle() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void DrawIndexed(Ref<VertexArray> vertexArray) = 0;

	protected:
		bool m_initialized = false;

		// TODO: more like... render pipeline (raster / raytracing)
		//Pipeline m_pipeline;
	};
}