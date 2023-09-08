#include "precompiled.h"
#include "Renderer.h"

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
	Ref<Framebuffer>	Renderer::s_framebuffer;

	void Renderer::Init()
	{
		s_shaderLibrary.Load("geometry", "shaders/geometry.vert", "shaders/geometry.frag");	
		s_shaderLibrary.Load("framebuffer", "shaders/framebuffer.vert", "shaders/framebuffer.frag");
	}
	void Renderer::Update(float deltaTime)
	{
		s_scene->m_camera.UpdateViewProjectionMatrix(s_graphicsAPI->GetAspectRatio());
		for (auto& entity : s_scene->m_entities)
		{
			entity.UpdateModelMatrix(s_scene->m_camera.m_viewProj);
		}

		// === Record Scene Framebuffer
		s_framebuffer->Bind();
		auto& geometryShader = s_shaderLibrary.Get("geometry");
		geometryShader->Bind();
		
		s_graphicsAPI->BeginGeometryPass();
		for (auto& entity : s_scene->m_entities)
		{
			geometryShader->UploadMat4("mvp", entity.m_ubo.mvp);
			s_graphicsAPI->DrawIndexed(entity);
		}
		s_graphicsAPI->EndPass();

		geometryShader->Unbind();
		s_framebuffer->Unbind();
		// ===

		s_graphicsAPI->BeginFrame();

		// === Draw Scene Framebuffer To Render Target
		/*auto& framebufferShader = s_shaderLibrary.Get("framebuffer");
		framebufferShader->Bind();
		s_framebuffer->Draw();
		framebufferShader->Unbind();*/
		// ===

		// === ImGui
		s_graphicsAPI->BeginGUIPass();
		Application::Get().m_guiLayer->Draw();
		s_graphicsAPI->EndPass();
		// ===

		s_graphicsAPI->EndFrame();
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

			// shutdown old api
			if (s_graphicsAPI->IsInitialized())
			{
				shutdownFn();

				s_graphicsAPI->Shutdown();

				EK_ASSERT((s_graphicsAPI == nullptr),
					"When switching api, old api ({0}) was not deleted", (int)apiType);
			}
		}

		// init new api
		s_apiType = apiType;
		s_graphicsAPI = GraphicsAPI::Create();

		s_scene = Application::Get().GetScene();
		if (!s_graphicsAPI->IsInitialized())
		{
			s_graphicsAPI->Init();
			initFn();

			// TEMP ///
			FramebufferInfo fbInfo;
			fbInfo.width = 512;
			fbInfo.height = 512;
			fbInfo.colorAttachmentInfos = {
				{ FramebufferTextureFormat::RGBA8 }
			};
			fbInfo.depthAttachmentInfo = { FramebufferTextureFormat::Depth };
			s_framebuffer = Framebuffer::Create(fbInfo);
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