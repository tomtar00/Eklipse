#include "precompiled.h"
#include "Renderer.h"

#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	Renderer::Renderer() : m_graphicsAPI(nullptr), m_scene(nullptr) 
	{
		s_instance = this;

		// TODO: Pick default graphics settings
		// RendererSettings::msaaSamples = GetMaxUsableSampleCount();
	}
	Renderer::~Renderer()
	{
		delete m_graphicsAPI;
	}

	Renderer& Renderer::Get()
	{
		return *s_instance;
	}

	void Renderer::Update(float deltaTime)
	{
		m_scene->m_camera.OnUpdate(m_graphicsAPI->GetAspectRatio());

		for (auto& model : m_scene->m_geometry)
		{
			model.OnUpdate(m_scene->m_camera.m_viewProj);
		}

		// TODO: split method for better abstraction
		m_graphicsAPI->DrawFrame();
		m_graphicsAPI->DrawGUI();
	}

	void Renderer::PostMainLoop()
	{
		m_graphicsAPI->OnPostLoop();
	}

	ApiType Renderer::GetAPI()
	{
		return m_apiType;
	}

	void Renderer::SetAPI(ApiType apiType, std::function<void()> shutdownFn, std::function<void()> initFn)
	{
		if (apiType == ApiType::None)
		{
			EK_CORE_ERROR("Cannot set graphics API to None");
			return;
		}

		if (m_graphicsAPI != nullptr)
		{
			if (apiType == m_apiType && m_graphicsAPI->IsInitialized())
			{
				EK_CORE_WARN("{0} API already set and initialized!", (int)apiType);
				return;
			}

			// shutdown old api
			if (m_graphicsAPI->IsInitialized())
			{
				shutdownFn();

				m_graphicsAPI->Shutdown();
				delete m_graphicsAPI;

				EK_ASSERT((m_graphicsAPI == nullptr),
					"When switching api, old api ({0}) was not deleted", (int)apiType);
			}
		}		

		// init new api
		switch (apiType)
		{
			case ApiType::Vulkan:
			{
				m_graphicsAPI = new Vulkan::VulkanAPI();
				break;
			}
			default:
			{
				EK_ASSERT(false, "API {0} not implemented!", (int)apiType);
				break;
			}
		}
		
		m_apiType = apiType;

		m_scene = Application::Get().GetScene();
		if (!m_graphicsAPI->IsInitialized())
		{
			m_graphicsAPI->Init(m_scene);
			initFn();
		}
		else
			EK_ASSERT(false, "API {0} not initialized!", (int)apiType);
	}
}
