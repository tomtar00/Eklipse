#pragma once
#include "Pipeline.h"
#include <Eklipse/Scene/Scene.h>

namespace Eklipse
{
	class GraphicsAPI
	{
	public:
		GraphicsAPI();

		virtual void Init(Scene* scene) = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void DrawFrame() = 0;

		virtual float GetAspectRatio() = 0;

		Scene* GetScene();
		bool IsInitialized();

		static Ref<GraphicsAPI> Create();

	protected:
		Scene* m_scene;
		bool m_initialized = false;

		// TODO: more like... render pipeline (raster / raytracing)
		//Pipeline m_pipeline;
	};
}