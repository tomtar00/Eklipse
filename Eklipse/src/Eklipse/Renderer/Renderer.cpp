#include "precompiled.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Settings.h"
#include "Material.h"

#include <Eklipse/Scene/Components.h>
#include <Eklipse/Core/Application.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>

namespace Eklipse
{
	ViewportSize		g_viewportSize = { 512, 512 };
	float				g_aspectRatio  = 1.0f;

	ApiType				Renderer::s_apiType;
	Ref<Viewport>		Renderer::s_viewport;

	// TODO: Remove
	static Ref<UniformBuffer>	s_cameraUniformBuffer;
	static Ref<UniformBuffer>	s_transformUniformBuffer;
	//

	void Renderer::Init()
	{
		s_cameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 0);
		s_transformUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 1);
	}	
	void Renderer::Shutdown()
	{
		RenderCommand::API->Shutdown();
		ShaderLibrary::Dispose();
	}

	void Renderer::BeginFrame(Camera& camera, Transform& cameraTransform)
	{
		EK_PROFILE();

		camera.UpdateViewProjectionMatrix(cameraTransform, g_aspectRatio);
		auto& viewProjection = camera.GetViewProjectionMatrix();
		s_cameraUniformBuffer->SetData(&viewProjection, sizeof(glm::mat4));

		RenderCommand::API->BeginFrame();
	}
	void Renderer::BeginRenderPass(Ref<Framebuffer> framebuffer)
	{
		EK_PROFILE();

		framebuffer->Bind();
	}
	void Renderer::RenderMeshes(Scene& scene)
	{
		auto view = scene.GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto& entity : view)
		{
			auto& [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);

			auto& transform = transformComponent.GetTransformMatrix();
			s_transformUniformBuffer->SetData(&transform, sizeof(glm::mat4));
			RenderCommand::DrawIndexed(meshComponent.mesh->GetVertexArray(), meshComponent.material);
		}
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
		if (s_viewport->HasFlags(VIEWPORT_FULLSCREEN))
		{
			s_viewport->Resize(width, height);
		}
	}
	void Renderer::OnMultiSamplingChanged(uint32_t numSamples)
	{
		FramebufferInfo fbInfo{};
		fbInfo.width = g_viewportSize.width;
		fbInfo.height = g_viewportSize.height;
		fbInfo.numSamples = numSamples;
		fbInfo.colorAttachmentInfos = s_viewport->GetCreateInfo().framebufferInfo.colorAttachmentInfos;
		fbInfo.depthAttachmentInfo = s_viewport->GetCreateInfo().framebufferInfo.depthAttachmentInfo;

		ViewportCreateInfo vCreateInfo{};
		vCreateInfo.flags = s_viewport->GetCreateInfo().flags;
		vCreateInfo.framebufferInfo = fbInfo;

		s_viewport.reset();
		s_viewport = Viewport::Create(vCreateInfo);
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
		
		/*FramebufferInfo fbInfo{};
		fbInfo.width = g_viewportSize.width;
		fbInfo.height = g_viewportSize.height;
		fbInfo.numSamples = RendererSettings::GetMsaaSamples();
		fbInfo.colorAttachmentInfos = {{ ImageFormat::RGBA8 }};
		fbInfo.depthAttachmentInfo = { ImageFormat::D24S8 };

		ViewportCreateInfo vCreateInfo{};
		vCreateInfo.flags = VIEWPORT_BLIT_FRAMEBUFFER;
		vCreateInfo.framebufferInfo = fbInfo;

		s_viewport.reset();
		s_viewport = Viewport::Create(vCreateInfo);*/

		Application::Get().OnInitAPI(s_apiType);
	}
	void Renderer::SetSceneFramebuffer(Ref<Framebuffer> framebuffer)
	{
		RenderCommand::API->SetSceneFramebuffer(framebuffer);
	}
	void Renderer::SetGUIFramebuffer(Ref<Framebuffer> framebuffer)
	{
		RenderCommand::API->SetGUIFramebuffer(framebuffer);
	}
}