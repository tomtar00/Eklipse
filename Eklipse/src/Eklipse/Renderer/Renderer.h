#pragma once
#include "Shader.h"
#include "Framebuffer.h"
#include "Buffers.h"
#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Scene/Camera.h>
#include <Eklipse/Utils/Yaml.h>

namespace Eklipse
{
	const constexpr uint32_t API_TYPE_COUNT = 2;
	enum class ApiType
	{
		Vulkan	= 0,
		OpenGL	= 1
	};
	
	const String APITypeToString(ApiType apiType);

	struct RendererSettings
	{
		bool Vsync = false;
		int MsaaSamplesIndex = 0;

		int GetMsaaSamples() const { return 1 << MsaaSamplesIndex; }
	};

	static class EK_API Renderer
	{
	public:
		static bool Init();
		static void InitParameters();
		static void WaitDeviceIdle();
		static void Shutdown();

		// Render stages
		static void BeginFrame();
		static void BeginDefaultRenderPass();
		static void EndDefaultRenderPass();
		static void BeginRenderPass(Framebuffer* framebuffer);
		static void EndRenderPass(Framebuffer* framebuffer);
		static void Submit();

		// Render calls
		static void RenderScene(Scene* scene, Camera& camera, Transform& cameraTransform);
		static void RenderScene(Scene* scene);

		// Events
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnMultiSamplingChanged(uint32_t numSamples);
		static void OnVsyncChanged(bool enabled);

		// Getters / Setters
		static ApiType GetAPI();
		static void SetStartupAPI(ApiType apiType);
		static void SetAPI(ApiType apiType);

		static RendererSettings& GetSettings();

		// Uniform buffers
		static Ref<UniformBuffer> CreateUniformBuffer(const String& uniformBufferName, const size_t size, const uint32_t binding);
		static Ref<UniformBuffer> GetUniformBuffer(const String& uniformBufferName);

		// Settings
		static void SerializeRendererSettings(YAML::Emitter& out);
		static void DeserializeRendererSettings(const YAML::Node& data);

	private:
		static ApiType s_apiType;
		static RendererSettings s_settings;
		static std::unordered_map<String, Ref<UniformBuffer>, std::hash<String>>	s_uniformBufferCache;
	};
}