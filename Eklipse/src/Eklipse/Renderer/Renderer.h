#pragma once
#include "GraphicsAPI.h"

namespace Eklipse
{
	enum class ApiType
	{
		None,
		Vulkan
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		Renderer& Get();

		void Update(float deltaTime);
		void PostMainLoop();

		ApiType GetAPI();
		void SetAPI(ApiType apiType, std::function<void()> shutdownFn, std::function<void()> initFn);

	private:
		inline static Renderer* s_instance = nullptr;

		ApiType m_apiType;
		Scene* m_scene;
		GraphicsAPI* m_graphicsAPI = nullptr;
	};
}