#pragma once
#include "GraphicsAPI.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Viewport.h"

namespace Eklipse
{
	enum class ApiType
	{
		None,
		Vulkan,
		OpenGL
	};

	class Renderer
	{
	public:
		static void Init();

		static void Update(float deltaTime);
		static void Shutdown();

		static ApiType GetAPI();
		static void SetStartupAPI(ApiType apiType);
		static void SetAPI(ApiType apiType, std::function<void()> shutdownFn, std::function<void()> initFn);

		static ShaderLibrary& GetShaderLibrary();

	private:
		static ApiType s_apiType;
		static Scene* s_scene;
		static ShaderLibrary s_shaderLibrary;
		static Ref<GraphicsAPI> s_graphicsAPI;

		static Ref<Viewport> s_viewport;
	};
}