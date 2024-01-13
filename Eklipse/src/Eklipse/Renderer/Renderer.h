#pragma once
#include "Shader.h"
#include "Framebuffer.h"
#include "Buffers.h"
#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Scene/Camera.h>

namespace Eklipse
{
	const constexpr uint32_t API_TYPE_COUNT = 2;
	enum class ApiType
	{
		Vulkan	= 0,
		OpenGL	= 1
	};

	class Renderer
	{
	public:
		static bool Init();
		static void InitParameters();
		static void WaitDeviceIdle();
		static void Shutdown();

		// Render stages
		static void BeginFrame();
		static void BeginRenderPass(Ref<Framebuffer> framebuffer);
		static void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform);
		static void RenderScene(Ref<Scene> scene);
		static void EndRenderPass(Ref<Framebuffer> framebuffer);
		static void Submit();

		// Events
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnMultiSamplingChanged(uint32_t numSamples);

		// Getters / Setters
		static ApiType GetAPI();
		static void SetStartupAPI(ApiType apiType);
		static void SetAPI(ApiType apiType);

		// Uniform buffers
		static Ref<UniformBuffer> CreateUniformBuffer(const std::string& uniformBufferName, const size_t size, const uint32_t binding);
		static Ref<UniformBuffer> GetUniformBuffer(const std::string& uniformBufferName);

	private:
		static ApiType s_apiType;
		static std::unordered_map<std::string, Ref<UniformBuffer>, std::hash<std::string>>	s_uniformBufferCache;
	};
}