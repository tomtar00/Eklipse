#pragma once
#include "GraphicsAPI.h"

namespace Eklipse
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void Update();
		void PostMainLoop();

		ApiType GetAPI();
		void SetAPI(ApiType api);

	private:
		GraphicsAPI* m_graphicsAPI = nullptr;
	};
}