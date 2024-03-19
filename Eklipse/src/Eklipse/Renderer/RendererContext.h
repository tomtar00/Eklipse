#pragma once
#include "Shader.h"
#include "Material.h"
#include "VertexArray.h"
#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Scene/Entity.h>
#include <Eklipse/Scene/Camera.h>
#include <Eklipse/Renderer/ComputeShader.h>

namespace Eklipse
{
	class EK_API RendererContext
	{
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void InitSSBOs() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnCompute(float deltaTime) {}
		virtual void OnWindowResize(uint32_t width, uint32_t height) {}
		virtual void OnMeshAdded(Entity entity) {}
		virtual void OnSphereAdded(Entity entity) {}
		virtual void OnSceneChanged() {}
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) = 0;
	};

	// === Rasterization ===
	class EK_API RasterizationContext : public RendererContext
	{
	public:
		virtual void Init() {}
		virtual void Shutdown() {}
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) override;
	};

	// === Ray Tracing ===
	class EK_API RayTracingContext : public RendererContext
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void InitSSBOs() override;
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnCompute(float deltaTime) override;
		virtual void OnWindowResize(uint32_t width, uint32_t height);
		virtual void OnMeshAdded(Entity entity) override;
		virtual void OnSphereAdded(Entity entity) override;
		virtual void OnSceneChanged() override;
		virtual void RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform) override;

	public:
		void SetAccumulate(bool accumulate);
		void SetRaysPerPixel(uint32_t raysPerPixel);
		void SetMaxBounces(uint32_t maxBounces);

		void RecompileShader();
		void RecompileTransformComputeShader();

	private:
		void InitMaterial();
		void ReconstructSceneBuffers();

	private:
		Ref<Shader> m_shader;
		Ref<Material> m_material;
		Ref<VertexArray> m_fullscreenQuad;

		Ref<ComputeShader> m_transComputeShader;

		uint32_t m_frameIndex = 0;
		uint32_t m_numTotalVertices;
		uint32_t m_numTotalIndices;
		uint32_t m_numTotalSpheres;
		uint32_t m_numTotalMeshes;

		glm::vec2 m_viewportSize;
		glm::vec2 m_lastViewportSize;
	};

	struct RayTracingMaterial
	{
		glm::vec3 albedo;
		float smoothness;
		glm::vec3 specularColor;
		float specularProb;		
		glm::vec3 emissionColor;
		float emissionStrength;
	};
	struct RayTracingSphereInfo
	{
		glm::vec3 position;
		float radius;
		uint32_t materialIndex;		float __padding0[3];
	};
	struct RayTracingMeshInfo
	{
		uint32_t vertexOffset;
		uint32_t vertexCount;
		uint32_t indexOffset;
		uint32_t indexCount;
		glm::vec3 boundMin;			float __padding0[1];
		glm::vec3 boundMax;
		uint32_t materialIndex;
	};
}