#pragma once
#include "Shader.h"
#include "Framebuffer.h"
#include "Buffers.h"
#include "Pipeline.h"
#include "Shader.h"
#include "Material.h"
#include "GraphicsAPI.h"
#include "RendererContext.h"

#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Scene/Camera.h>
#include <Eklipse/Utils/Yaml.h>

namespace Eklipse
{
	struct RendererSettings
	{
		bool Vsync = false;
		int MsaaSamplesIndex = 0;

		GraphicsAPI::Type GraphicsAPIType = GraphicsAPI::Type::Vulkan;

		Pipeline::Type PipelineType = Pipeline::Type::Resterization;
		Pipeline::TopologyMode PipelineTopologyMode = Pipeline::TopologyMode::Triangle;

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

		// State changing
		static GraphicsAPI::Type GetGraphicsAPIType();
		static void SetGraphicsAPIType(GraphicsAPI::Type apiType);
		static void SetPipelineTopologyMode(Pipeline::TopologyMode mode);
		static void SetPipelineType(Pipeline::Type type);

		// Uniform buffers
		static Ref<UniformBuffer> CreateUniformBuffer(const String& uniformBufferName, const size_t size, const uint32_t binding);
		static Ref<UniformBuffer> GetUniformBuffer(const String& uniformBufferName);
		// Storage buffers
		static Ref<StorageBuffer> CreateStorageBuffer(const String& storageBufferName, const size_t size, const uint32_t binding);
		static Ref<StorageBuffer> GetStorageBuffer(const String& storageBufferName);

		// Settings
		static const RendererSettings& GetSettings();
		static void SerializeRendererSettings(YAML::Emitter& out);
		static void DeserializeRendererSettings(const YAML::Node& data);

	private:
		static std::unordered_map<String, Ref<UniformBuffer>, std::hash<String>> s_uniformBufferCache;
		static std::unordered_map<String, Ref<StorageBuffer>, std::hash<String>> s_storageBufferCache;
		
		static RendererSettings s_settings;
		static Unique<RendererContext> s_rendererContext;
		static Ref<UniformBuffer> s_cameraUniformBuffer;
		static Ref<Framebuffer> s_defaultFramebuffer;
	};
}