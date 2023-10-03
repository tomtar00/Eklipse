#pragma once
#include "Shader.h"
#include "Viewport.h"
#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Scene/Camera.h>

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
		None	= 0,
		Vulkan	= 1,
		OpenGL	= 2
	};

	class Renderer
	{
	public:
		static void Init();

		static void RecordViewport(Scene& scene, Camera& camera, float deltaTime);
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnMultiSamplingChanged(uint32_t numSamples);

		static ApiType GetAPI();
		static void SetStartupAPI(ApiType apiType);
		static void SetAPI(ApiType apiType);

		inline static ShaderLibrary& GetShaderLibrary() { return s_shaderLibrary; }
		inline static Ref<Viewport> GetViewport() { return s_viewport; }

	private:
		static ApiType s_apiType;
		static ShaderLibrary s_shaderLibrary;
		static Ref<Viewport> s_viewport;
	};
}