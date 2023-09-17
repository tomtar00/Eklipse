#pragma once
#include "GraphicsAPI.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Viewport.h"

namespace Eklipse
{
	struct ViewportSize
	{
		uint32_t width{ 512 }, height{ 512 };
	};
	extern ViewportSize g_viewportSize;
	extern float g_aspectRatio;

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

		static void OnWindowResize(uint32_t width, uint32_t height);

		static ApiType GetAPI();
		static void SetStartupAPI(ApiType apiType);
		static void SetAPI(ApiType apiType, std::function<void()> shutdownFn, std::function<void()> initFn);

		static ShaderLibrary& GetShaderLibrary();

	private:
		static ApiType s_apiType;
		static Scene* s_scene;
		static ShaderLibrary s_shaderLibrary;
		static Ref<Viewport> s_viewport;
	};
}