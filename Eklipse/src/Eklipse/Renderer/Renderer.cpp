#include "precompiled.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Settings.h"
#include "Material.h"

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Scene/Components.h>
#include <Eklipse/Core/Application.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>

namespace Eklipse
{
	ApiType	Renderer::s_apiType = ApiType::Vulkan;

	static Ref<UniformBuffer> s_cameraUniformBuffer;
	std::unordered_map<std::string, Ref<UniformBuffer>, std::hash<std::string>>	Renderer::s_uniformBufferCache;

	void Renderer::Init()
	{
		RenderCommand::API.reset();
		RenderCommand::API = GraphicsAPI::Create();
		RenderCommand::API->Init();
	}
	void Renderer::InitParameters()
	{
		s_cameraUniformBuffer = Renderer::CreateUniformBuffer("uCamera", sizeof(glm::mat4), 0);
	}

	void Renderer::WaitDeviceIdle()
	{
		RenderCommand::API->WaitDeviceIdle();
	}

	void Renderer::Shutdown()
	{
		for (auto&& [name, uniformBuffer] : s_uniformBufferCache)
		{
			uniformBuffer->Dispose();
		}
		s_uniformBufferCache.clear();
		RenderCommand::API->Shutdown();
	}

	void Renderer::BeginFrame()
	{
		EK_PROFILE();

		Stats::Get().Reset();
		RenderCommand::API->BeginFrame();
	}
	void Renderer::BeginRenderPass(Ref<Framebuffer> framebuffer)
	{
		EK_PROFILE();

		framebuffer->Bind();
	}
	void Renderer::RenderScene(Ref<Scene> scene, Camera& camera, Transform& cameraTransform)
	{
		EK_PROFILE();

		camera.UpdateViewProjectionMatrix(cameraTransform, g_sceneFramebuffer->GetAspectRatio());
		auto& viewProjection = camera.GetViewProjectionMatrix();
		s_cameraUniformBuffer->SetData(&viewProjection, sizeof(glm::mat4));

		// Geometry
		auto view = scene->GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto& entity : view)
		{
			auto [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);

#ifdef EK_DEBUG
			if (meshComponent.mesh == nullptr || meshComponent.material == nullptr || !meshComponent.material->IsValid()) continue;
#endif

			glm::mat4& modelMatrix = transformComponent.GetTransformMatrix();
			meshComponent.material->SetConstant("pConstants", "Model", &modelMatrix[0][0], sizeof(glm::mat4));
			RenderCommand::DrawIndexed(meshComponent.mesh->GetVertexArray(), meshComponent.material);
		}

		// ...
	}
	void Renderer::RenderScene(Ref<Scene> scene)
	{
		auto camera = scene->GetMainCamera();
		auto cameraTransform = scene->GetMainCameraTransform();

#ifdef EK_DEBUG
		if (!camera)
		{
			EK_CORE_ERROR("No main camera present on the scene!");
			return;
		}
		else if (!cameraTransform)
		{
			EK_CORE_ERROR("Main camera transform is null!");
		}
		else
#endif
			RenderScene(scene, *camera, *cameraTransform);
	}
	void Renderer::EndRenderPass(Ref<Framebuffer> framebuffer)
	{
		EK_PROFILE();

		framebuffer->Unbind();
	}
	void Renderer::Submit()
	{
		EK_PROFILE();

		RenderCommand::API->EndFrame();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		g_defaultFramebuffer->Resize(width, height);
	}
	void Renderer::OnMultiSamplingChanged(uint32_t numSamples)
	{
		// TODO: Works with Vulkan, but not with OpenGL
		// ImGui doesn't work with multisampled GLTexture2Ds as ImGui::Image() input

		/*
		auto& fbInfo = g_sceneFramebuffer->GetInfo();
		fbInfo.numSamples = numSamples;
		g_sceneFramebuffer->Resize(fbInfo.width, fbInfo.height);
		*/
	}

	ApiType Renderer::GetAPI()
	{
		return s_apiType;
	}
	void Renderer::SetStartupAPI(ApiType apiType)
	{
		s_apiType = apiType;
	}
	void Renderer::SetAPI(ApiType apiType)
	{
		EK_ASSERT(apiType != ApiType::None, "Cannot set graphics API to None");

		s_apiType = apiType;
	}
	Ref<UniformBuffer> Renderer::CreateUniformBuffer(const std::string& uniformBufferName, const size_t size, const uint32_t binding)
	{
		if (s_uniformBufferCache.find(uniformBufferName) != s_uniformBufferCache.end())
		{
			return s_uniformBufferCache[uniformBufferName];
		}

		Ref<UniformBuffer> uniformBuffer = UniformBuffer::Create(size, binding);
		s_uniformBufferCache[uniformBufferName] = uniformBuffer;
		EK_CORE_INFO("Created uniform buffer '{0}' with size {1} and binding {2}", uniformBufferName, size, binding);
		return uniformBuffer;
	}
	Ref<UniformBuffer> Renderer::GetUniformBuffer(const std::string& uniformBufferName)
	{
		if (s_uniformBufferCache.find(uniformBufferName) != s_uniformBufferCache.end())
		{
			return s_uniformBufferCache[uniformBufferName];
		}

		EK_ASSERT(false, "Uniform buffer '{0}' not found", uniformBufferName);
		return nullptr;
	}
}