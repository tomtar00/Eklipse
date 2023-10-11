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
		static void Shutdown();

		// Render stages
		static void BeginFrame(Camera& camera, Transform& cameraTransform);
		static void BeginRenderPass(Ref<Framebuffer> framebuffer);
		static void RenderMeshes(Scene& scene);
		static void EndRenderPass(Ref<Framebuffer> framebuffer);
		static void Submit();

		// Events
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnMultiSamplingChanged(uint32_t numSamples);

		// Getters / Setters
		static ApiType GetAPI();
		static void SetStartupAPI(ApiType apiType);
		static void SetAPI(ApiType apiType);
		static void SetSceneFramebuffer(Ref<Framebuffer> framebuffer);
		static void SetGUIFramebuffer(Ref<Framebuffer> framebuffer);

		inline static Ref<Viewport> GetViewport() { return s_viewport; }

	private:
		static ApiType s_apiType;
		static Ref<Viewport> s_viewport;
	};
}