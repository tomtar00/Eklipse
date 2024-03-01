#pragma once
#include "Shader.h"
#include "Framebuffer.h"
#include "Buffers.h"
#include "GraphicsAPI.h"
#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Scene/Camera.h>
#include <Eklipse/Utils/Yaml.h>
#include <Eklipse/Renderer/Pipeline.h>

namespace Eklipse
{
	struct RendererSettings
	{
		bool Vsync = false;
		int MsaaSamplesIndex = 0;

		GraphicsAPI::Type GraphicsAPIType = GraphicsAPI::Type::Vulkan;

		Pipeline::Type PipelineType = Pipeline::Type::Resterization;
		Pipeline::Mode PipelineMode = Pipeline::Mode::Triangle;

		int GetMsaaSamples() const { return 1 << MsaaSamplesIndex; }
	};

	class EK_API RendererAPI
	{
	public:
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) = 0;
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
		static void UpdateViewProjection(Camera& camera, Transform& cameraTransform);
		static void BeginRenderPass(Framebuffer* framebuffer);
		static void EndRenderPass(Framebuffer* framebuffer);
		static void Submit();

		// Render calls
		static void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform);
	static void RenderScene(Ref<Scene> scene);

		// Events
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnMultiSamplingChanged(uint32_t numSamples);
		static void OnVsyncChanged(bool enabled);

		// Getters / Setters
		static GraphicsAPI::Type GetAPI();
		static void SetAPI(GraphicsAPI::Type apiType);

		// Uniform buffers
		static Ref<UniformBuffer> CreateUniformBuffer(const String& uniformBufferName, const size_t size, const uint32_t binding);
		static Ref<UniformBuffer> GetUniformBuffer(const String& uniformBufferName);
		// Storage buffers
		static Ref<StorageBuffer> CreateStorageBuffer(const String& storageBufferName, const size_t size, const uint32_t binding);
		static Ref<StorageBuffer> GetStorageBuffer(const String& storageBufferName);

		// Settings
		static RendererSettings& GetSettings();
		static void SerializeRendererSettings(YAML::Emitter& out);
		static void DeserializeRendererSettings(const YAML::Node& data);

	private:
		static RendererSettings s_settings;
		static std::unordered_map<String, Ref<UniformBuffer>, std::hash<String>> s_uniformBufferCache;
		static std::unordered_map<String, Ref<StorageBuffer>, std::hash<String>> s_storageBufferCache;
		static Ref<UniformBuffer> s_cameraUniformBuffer;
		static Ref<Framebuffer> s_defaultFramebuffer;
	};
}