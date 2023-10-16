#include "precompiled.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Settings.h"
#include "Material.h"

#include <Eklipse/Scene/Assets.h>
#include <Eklipse/Scene/Components.h>
#include <Eklipse/Core/Application.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>

namespace Eklipse
{
	ApiType						Renderer::s_apiType;

	static Ref<UniformBuffer>	s_cameraUniformBuffer;
	static Ref<UniformBuffer>	s_transformUniformBuffer;

	void Renderer::Init()
	{
		s_cameraUniformBuffer		= Assets::GetUniformBuffer("uCamera");
		s_transformUniformBuffer	= Assets::GetUniformBuffer("uTransform");
	}	
	void Renderer::Shutdown() // TODO: Call this when switching graphics API
	{
		RenderCommand::API->WaitDeviceIdle();
		Assets::Shutdown();
		RenderCommand::API->Shutdown();
	}

	void Renderer::BeginFrame(Camera& camera, Transform& cameraTransform)
	{
		EK_PROFILE();

		camera.UpdateViewProjectionMatrix(cameraTransform, g_sceneFramebuffer->GetAspectRatio());
		auto& viewProjection = camera.GetViewProjectionMatrix();
		s_cameraUniformBuffer->SetData(&viewProjection, sizeof(glm::mat4));

		RenderCommand::API->BeginFrame();
	}
	void Renderer::BeginRenderPass(Ref<Framebuffer> framebuffer)
	{
		EK_PROFILE();

		framebuffer->Bind();
	}
	void Renderer::RenderScene(Scene& scene)
	{
		// Geometry
		auto view = scene.GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto& entity : view)
		{
			auto& [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);

			auto& transform = transformComponent.GetTransformMatrix();
			s_transformUniformBuffer->SetData(&transform, sizeof(glm::mat4));
			RenderCommand::DrawIndexed(meshComponent.mesh->GetVertexArray(), meshComponent.material);
		}

		// ...
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

		if (RenderCommand::API != nullptr)
		{
			if (apiType == s_apiType && RenderCommand::API->IsInitialized())
			{
				EK_CORE_WARN("{0} API already set and initialized!", (int)apiType);
				return;
			}

			if (RenderCommand::API->IsInitialized())
			{
				Application::Get().OnShutdownAPI();
				RenderCommand::API->Shutdown();
			}
		}

		s_apiType = apiType;
		RenderCommand::API.reset();
		RenderCommand::API = GraphicsAPI::Create();
		RenderCommand::API->Init();
		
		Application::Get().OnInitAPI(s_apiType);
	}
}