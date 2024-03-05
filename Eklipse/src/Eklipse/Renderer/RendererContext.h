#pragma once
#include "Shader.h"
#include "Material.h"
#include "VertexArray.h"
#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Scene/Camera.h>

namespace Eklipse
{
	class EK_API RendererContext
	{
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) = 0;
	};

	// === Rasterization ===
	class EK_API RasterizationContext : public RendererContext
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) override;
	};

	// === Ray Tracing ===
	class EK_API RayTracingContext : public RendererContext
	{
	public:
		struct Settings
		{
			int raysPerPixel = 1;
			int maxBounces = 4;

			glm::vec3 skyColorHorizon = { 1.0f, 1.0f, 1.0f };
			glm::vec3 skyColorZenith = { 0.07f, 0.36f, 0.72f };
			glm::vec3 groundColor = { 0.35f, 0.3f, 0.35f };
			glm::vec3 sunColor = { 1.0f, 1.0f, 0.8f };
			glm::vec3 sunDirection = { 0.0f, 0.3f, -1.0f };

			float sunFocus = 500.0f;
			float sunIntensity = 200.0f;
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
	struct RayTracingMaterial
	{
		glm::vec3 albedo;
		float __pad0;
		float smoothness;
		float specularProb;
		glm::vec3 specularColor;
		float __pad1;
		glm::vec3 emissionColor;
		float __pad2;
		float emissionStrength;
	};
	struct RayTracingMeshInfo
	{
		uint32_t vertexOffset;
		uint32_t vertexCount;
		uint32_t indexOffset;
		uint32_t indexCount;
		uint32_t materialIndex;
		glm::vec3 boundMin;
		float __pad0;
		glm::vec3 boundMax;
		float __pad1;
	};
}