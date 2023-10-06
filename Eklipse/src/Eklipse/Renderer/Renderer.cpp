#include "precompiled.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Settings.h"

#include <Eklipse/Scene/Components.h>
#include <Eklipse/Core/Application.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>

namespace Eklipse
{
	ViewportSize		g_viewportSize = { 512, 512 };
	float				g_aspectRatio  = 0.5f;

	ApiType				Renderer::s_apiType;
	ShaderLibrary		Renderer::s_shaderLibrary;
	Ref<Viewport>		Renderer::s_viewport;

	// TODO: Remove
	static Ref<Shader>			s_meshShader;
	static Ref<Shader>			s_spriteShader;
	static Ref<UniformBuffer>	s_cameraUniformBuffer;
	static Ref<UniformBuffer>	s_transformUniformBuffer;
	//

	void Renderer::Init()
	{
		s_meshShader = s_shaderLibrary.Load("Assets/Shaders/mesh.glsl");
		s_spriteShader = s_shaderLibrary.Load("Assets/Shaders/sprite.glsl");

		s_cameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 0);
		s_transformUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 1);
	}
	void Renderer::RecordViewport(Scene& scene, Camera& camera, float deltaTime)
	{
		EK_PROFILE_NAME("Record Scene");	

		auto& viewProjection = camera.GetViewProjectionMatrix();
		s_cameraUniformBuffer->SetData(&viewProjection, sizeof(glm::mat4));
		
		s_viewport->BindFramebuffer();
		
		RenderCommand::API->BeginGeometryPass();
		s_meshShader->Bind();
		auto view = scene.GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto& entity : view)
		{
			auto& [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);

			//s_meshShader->UploadInt("texSampler", 0);
			auto& transform = transformComponent.GetTransformMatrix(viewProjection);
			s_transformUniformBuffer->SetData(&transform, sizeof(glm::mat4));
			RenderCommand::DrawIndexed(meshComponent.mesh.GetVertexArray(), meshComponent.mesh.GetTexture());
		}
		RenderCommand::API->EndPass();
		
		s_viewport->UnbindFramebuffer();
	}
	void Renderer::Shutdown()
	{
		RenderCommand::API->Shutdown();
		s_shaderLibrary.Dispose();
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
		Application::Get().OnInitAPI(s_apiType);

		FramebufferInfo fbInfo{};
		fbInfo.width = g_viewportSize.width;
		fbInfo.height = g_viewportSize.height;
		fbInfo.numSamples = RendererSettings::GetMsaaSamples();
		fbInfo.colorAttachmentInfos = {{ ImageFormat::RGBA8 }};
		fbInfo.depthAttachmentInfo = { ImageFormat::D24S8 };

		ViewportCreateInfo vCreateInfo{};
		vCreateInfo.flags = VIEWPORT_BLIT_FRAMEBUFFER;
		vCreateInfo.framebufferInfo = fbInfo;

		s_viewport.reset();
		s_viewport = Viewport::Create(vCreateInfo);
	}
}