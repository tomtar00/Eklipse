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
	}

	void Renderer::PostMainLoop()
	{
		m_graphicsAPI->OnPostLoop();
	}

	ApiType Renderer::GetAPI()
	{
		return m_apiType;
	}

	void Renderer::SetAPI(ApiType apiType)
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
				EK_CORE_WARN("{0} API already set and initialized!", STRINGIFY(apiType));
				return;
			}

			// shutdown old api
			if (m_graphicsAPI->IsInitialized())
			{
				Application::Get().m_debugLayer->Shutdown();
				delete Application::Get().m_debugLayer;

				m_graphicsAPI->Shutdown();
				delete m_graphicsAPI;

				EK_ASSERT((m_graphicsAPI == nullptr),
					"When switching api, old api ({0}) was not deleted", STRINGIFY(apiType));
			}
		}		

		// init new api
		switch (apiType)
		{
			case ApiType::Vulkan:
			{
				Vulkan::VkImGuiLayer* vkGui = new Vulkan::VkImGuiLayer(Application::Get().GetWindow());
				Application::Get().m_debugLayer = vkGui;
				m_graphicsAPI = new Vulkan::VulkanAPI(vkGui);
				break;
			}
			default:
			{
				EK_ASSERT(false, "API {0} not implemented!", STRINGIFY(apiType));
				break;
			}
		}
		
		m_apiType = apiType;

		m_scene = Application::Get().GetScene();
		if (!m_graphicsAPI->IsInitialized())
		{
			m_graphicsAPI->Init(m_scene);
			Application::Get().m_debugLayer->Init();
			Application::Get().m_debugLayer->AddPanel(Application::Get().m_debugPanel);
		}
		else
			EK_ASSERT(false, "API {0} not initialized!", STRINGIFY(apiType));
	}
}
