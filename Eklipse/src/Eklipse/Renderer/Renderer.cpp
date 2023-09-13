#include "precompiled.h"
#include "Renderer.h"
#include "Settings.h"

#include <Eklipse/Core/Application.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>

namespace Eklipse
{
	ApiType				Renderer::s_apiType;
	Scene*				Renderer::s_scene;
	ShaderLibrary		Renderer::s_shaderLibrary;
	Ref<GraphicsAPI>	Renderer::s_graphicsAPI;
	Ref<Viewport>		Renderer::s_viewport;

	static Ref<Shader> s_geometryShader;
	static Ref<Shader> s_framebufferShader;

	void Renderer::Init()
	{
		s_scene = Application::Get().GetScene();

		s_geometryShader = s_shaderLibrary.Load("geometry", "shaders/geometry.vert", "shaders/geometry.frag");
		s_framebufferShader = s_shaderLibrary.Load("framebuffer", "shaders/framebuffer.vert", "shaders/framebuffer.frag");
	}
	void Renderer::Update(float deltaTime)
	{
		s_scene->m_camera.UpdateViewProjectionMatrix(s_graphicsAPI->GetAspectRatio());
		for (auto& entity : s_scene->m_entities)
		{
			entity.UpdateModelMatrix(s_scene->m_camera.m_viewProj);
		}
		
		// =============== Record Scene
		s_viewport->Bind();

		s_geometryShader->Bind();
		s_geometryShader->UploadInt("texSampler", 0);
		
		s_graphicsAPI->BeginGeometryPass();
		for (auto& entity : s_scene->m_entities)
		{
			s_geometryShader->UploadMat4("mvp", entity.m_ubo.mvp);
			s_graphicsAPI->DrawIndexed(entity);
		}
		s_graphicsAPI->EndPass();

		s_geometryShader->Unbind();
		
		s_viewport->Unbind();
		// ==============================

		///////////////////////////////// FRAME
		s_graphicsAPI->BeginFrame();

		// =============== Draw Scene
		s_framebufferShader->Bind();
		s_viewport->Draw();
		s_framebufferShader->Unbind();
		// ==============================

		// =============== Draw ImGui
		s_graphicsAPI->BeginGUIPass();
		Application::Get().DrawGUI();
		s_graphicsAPI->EndPass();
		// ==============================

		s_graphicsAPI->EndFrame();
		////////////////////////////////
	}

	void Renderer::Shutdown()
	{
		s_graphicsAPI->Shutdown();
		s_shaderLibrary.Dispose();
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
		if (apiType == ApiType::None)
		{
			EK_CORE_ERROR("Cannot set graphics API to None");
			return;
		}

		if (s_graphicsAPI != nullptr)
		{
			if (apiType == s_apiType && s_graphicsAPI->IsInitialized())
			{
				EK_CORE_WARN("{0} API already set and initialized!", (int)apiType);
				return;
			}

			if (s_graphicsAPI->IsInitialized())
			{
				shutdownFn();
				s_graphicsAPI->Shutdown();
			}
		}

		s_apiType = apiType;
		s_graphicsAPI = GraphicsAPI::Create();

		if (!s_graphicsAPI->IsInitialized())
		{
			s_graphicsAPI->Init();
			initFn();

			// TEMP ///
			ViewportCreateInfo vCreateInfo{};
			vCreateInfo.flags = VIEWPORT_BLIT_FRAMEBUFFER;

			FramebufferInfo fbInfo{};
			fbInfo.width = 512;
			fbInfo.height = 512;
			fbInfo.numSamples = RendererSettings::GetMsaaSamples();
			fbInfo.colorAttachmentInfos = {{ FramebufferTextureFormat::RGBA8 }};
			fbInfo.depthAttachmentInfo = { FramebufferTextureFormat::Depth };

			vCreateInfo.framebufferInfo = fbInfo;

			s_viewport = Viewport::Create(vCreateInfo);
			//////////
		}
		else
			EK_ASSERT(false, "API {0} not initialized!", (int)apiType);
	}
	ShaderLibrary& Renderer::GetShaderLibrary()
	{
		return s_shaderLibrary;
	}
}