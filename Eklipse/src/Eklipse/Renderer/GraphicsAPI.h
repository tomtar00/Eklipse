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

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		//virtual void BeginGeometryPass() = 0;
		//virtual void BeginGUIPass() = 0;
		//virtual void EndPass() = 0;

		virtual void DrawIndexed(Ref<VertexArray> vertexArray) = 0;
		
		bool IsInitialized();

	protected:
		bool m_initialized = false;

		// TODO: more like... render pipeline (raster / raytracing)
		//Pipeline m_pipeline;
	};
}