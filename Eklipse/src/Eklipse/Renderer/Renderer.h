#pragma once
#include "Shader.h"
#include "Framebuffer.h"
#include "Buffers.h"
#include "Pipeline.h"
#include "Shader.h"
#include "Material.h"
#include "GraphicsAPI.h"
//#include "RendererContext.h"

#include <Eklipse/Scene/Entity.h>
#include <Eklipse/Scene/Camera.h>
#include <Eklipse/Utils/Yaml.h>

namespace Eklipse
{
    class RendererContext;
    class Scene;

	struct RendererSettings
	{
		GraphicsAPI::Type GraphicsAPIType = GraphicsAPI::Type::Vulkan;
		Pipeline::Type PipelineType = Pipeline::Type::Resterization;
		Pipeline::TopologyMode PipelineTopologyMode = Pipeline::TopologyMode::Triangle;

		// Common
		PresentMode presentMode		= PresentMode::IMMEDIATE;
		int MsaaSamplesIndex		= 0;
		glm::vec3 skyColorHorizon	= { 1.0f, 1.0f, 1.0f };
		glm::vec3 skyColorZenith	= { 0.07f, 0.36f, 0.72f };
		glm::vec3 groundColor		= { 0.35f, 0.3f, 0.35f };
		glm::vec3 sunColor			= { 1.0f, 1.0f, 0.8f };
		glm::vec3 sunDirection		= { 0.0f, 0.3f, -1.0f };
		float sunFocus				= 500.0f;
		float sunIntensity			= 100.0f;

		// Rasterization
		// ...

		// Ray Tracing
		bool accumulate;
		int raysPerPixel	= 1;
		int maxBounces		= 4;

		// Functions
		int GetMsaaSamples() const { return 1 << MsaaSamplesIndex; }
	};

	static class EK_API Renderer
	{
		friend class RasterizationContext;
		friend class RayTracingContext;

	public:
		static bool Init(GraphicsAPI::Type apiType);
		static void InitSSBOs();
		static void OnAPIHasInitialized();
		static void Shutdown();
		static void WaitDeviceIdle();

		// Render stages
		static void BeginFrame();
		static void UpdateViewProjection(Camera& camera, Transform& cameraTransform);
		static void OnUpdate(float deltaTime);
		static void BeginComputePass();
		static void OnCompute(float deltaTime);
		static void EndComputePass();
		static void BeginRenderPass(Framebuffer* framebuffer);
		static void EndRenderPass(Framebuffer* framebuffer);
		static void Submit();

		// Render calls
		static void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform);
		static void RenderScene(Ref<Scene> scene);

		// Events
		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnMultiSamplingChanged(Framebuffer* framebuffer, uint32_t numSamples);
		static void OnPresentModeChanged(PresentMode mode);
		static void OnMeshAdded(Entity entity);
		static void OnSphereAdded(Entity entity);
		static void OnSceneChanged();

		// State changing
		static GraphicsAPI::Type GetGraphicsAPIType();
		static GraphicsAPI::Type GetTargetGraphicsAPIType();
		static Pipeline::Type Renderer::GetPipelineType();
		static Ref<RendererContext> GetRendererContext();
		static void SetTargetGraphicsAPIType(GraphicsAPI::Type apiType);
		static void SetPipelineTopologyMode(Pipeline::TopologyMode mode);
		static void SetPipelineType(Pipeline::Type type);
		static void RequestPipelineTypeChange(Pipeline::Type type);

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
		static std::unordered_map<String, Ref<UniformBuffer>, std::hash<String>> s_uniformBufferCache;
		static std::unordered_map<String, Ref<StorageBuffer>, std::hash<String>> s_storageBufferCache;
		
		static GraphicsAPI::Type s_targetAPIType;
		static Pipeline::Type s_targetPipelineType;
		static bool s_pipelineTypeChangeRequeted;

		static RendererSettings s_settings;
		static Ref<RendererContext> s_rendererContext;
		static Ref<UniformBuffer> s_cameraUniformBuffer;
		static Ref<Framebuffer> s_defaultFramebuffer;
	};
}