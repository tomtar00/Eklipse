#pragma once
#include "Shader.h"
#include "Framebuffer.h"
#include "Buffers.h"
#include "Pipeline.h"
#include "Shader.h"
#include "Material.h"
#include "GraphicsAPI.h"

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

	class EK_API RendererContext
	{
	public:
	    virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) = 0;
	};
	
	class EK_API RasterizationContext : public RendererContext
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) override;
	};
	
	class EK_API RayTracingContext : public RendererContext
	{
	public:
		struct Settings 
		{
			uint32_t raysPerPixel	= 1;
			uint32_t maxBounces		= 4;

			glm::vec3 m_skyColorHorizon		= { 1.0f, 1.0f, 1.0f };
			glm::vec3 m_skyColorZenith		= { 0.07f, 0.36f, 0.72f };
			glm::vec3 m_groundColor			= { 0.35f, 0.3f, 0.35f };
			glm::vec3 m_sunColor			= { 1.0f, 1.0f, 0.8f };
			glm::vec3 m_sunDirection		= { 0.0f, 0.3f, -1.0f };

			float m_sunFocus				= 500.0f;
			float m_sunIntensity			= 200.0f;
		};

	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) override;
	
	private:
		uint32_t m_frameIndex = 0;
		Ref<Shader> m_rayTracingShader;
		Ref<Material> m_rayTracingMaterial;
		Ref<VertexArray> m_rayTracingQuad;
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