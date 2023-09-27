#include "precompiled.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Settings.h"

#include <Eklipse/Utils/Stats.h>
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
	Scene*				Renderer::s_scene;
	ShaderLibrary		Renderer::s_shaderLibrary;
	Ref<Viewport>		Renderer::s_viewport;

	// TODO: Remove
	static Ref<Shader>	s_geometryShader;
	static Ref<Shader>	s_framebufferShader;
	//

	void Renderer::Init()
	{
		s_scene = Application::Get().GetScene();

		s_geometryShader = s_shaderLibrary.Load("geometry", "shaders/geometry.vert", "shaders/geometry.frag");
		s_framebufferShader = s_shaderLibrary.Load("framebuffer", "shaders/framebuffer.vert", "shaders/framebuffer.frag");
	}
	void Renderer::DrawFrame(Camera& camera, float deltaTime)
	{
		EK_PROFILE_NAME("Renderer");
		Stats::Get().Reset();

		if (g_viewportSize.width == 0 || g_viewportSize.height == 0) return;

		//auto& cameraView = s_scene->GetRegistry().view<TransformComponent, CameraComponent>();
		//for (auto entity : cameraView)
		//{
		//	auto& [transformComponent, cameraComponent] = cameraView.get<TransformComponent, CameraComponent>(entity);

		//	cameraComponent.camera.UpdateViewProjectionMatrix(transformComponent.transform, g_aspectRatio);
		//	s_camera = &cameraComponent.camera; // TODO: Change this
		//}
		
		// =============== Record Scene
		s_viewport->BindFramebuffer();

		s_geometryShader->Bind();
		s_geometryShader->UploadInt("texSampler", 0);
		
		RenderCommand::API->BeginGeometryPass();
		auto view = s_scene->GetRegistry().view<TransformComponent, MeshComponent>();
		for (auto& entity : view)
		{
			auto& [transformComponent, meshComponent] = view.get<TransformComponent, MeshComponent>(entity);

			s_geometryShader->UploadMat4("mvp", transformComponent.GetTransformMatrix(camera.m_viewProj));
			RenderCommand::DrawIndexed(s_geometryShader, meshComponent.mesh.GetVertexArray(), meshComponent.mesh.GetTexture());
		}
		RenderCommand::API->EndPass();

		s_geometryShader->Unbind();
		
		s_viewport->UnbindFramebuffer();
		// ==============================

		///////////////////////////////// FRAME
		RenderCommand::API->BeginFrame();

		// =============== Draw Scene Fullscreen
		if (s_viewport->HasFlags(VIEWPORT_FULLSCREEN))
		{
			s_viewport->Bind();
			RenderCommand::DrawIndexed(s_framebufferShader, s_viewport->GetVertexArray());
		}
		// ==============================

		// =============== Draw ImGui
		RenderCommand::API->BeginGUIPass();
		Application::Get().DrawGUI();
		RenderCommand::API->EndPass();
		// ==============================

		RenderCommand::API->EndFrame();
		////////////////////////////////

		Stats::Get().Update(deltaTime);
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
	void Renderer::SetAPI(ApiType apiType, std::function<void()> shutdownFn, std::function<void()> initFn)
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
				shutdownFn();
				RenderCommand::API->Shutdown();
			}
		}

		s_apiType = apiType;
		RenderCommand::API.reset();
		RenderCommand::API = GraphicsAPI::Create();

		RenderCommand::API->Init();
		initFn();

		FramebufferInfo fbInfo{};
		fbInfo.width = g_viewportSize.width;
		fbInfo.height = g_viewportSize.height;
		fbInfo.numSamples = RendererSettings::GetMsaaSamples();
		fbInfo.colorAttachmentInfos = {{ FramebufferTextureFormat::RGBA8 }};
		fbInfo.depthAttachmentInfo = { FramebufferTextureFormat::DEPTH24STENCIL8 };

		ViewportCreateInfo vCreateInfo{};
		vCreateInfo.flags = VIEWPORT_BLIT_FRAMEBUFFER;
		vCreateInfo.framebufferInfo = fbInfo;

		s_viewport = Viewport::Create(vCreateInfo);
	}
}