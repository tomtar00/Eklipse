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

		std::vector<GuiLayerConfigInfo> guiLayerConfigs;
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
				for (auto& guiLayer : Application::Get().m_guiLayers)
				{
					guiLayerConfigs.push_back(guiLayer->GetConfig());
					guiLayer->Shutdown();
					delete guiLayer;
				}

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
				int i = 0;
				for (auto& guiLayer : Application::Get().m_guiLayers)
				{
					guiLayer = new Vulkan::VkImGuiLayer(Application::Get().GetWindow(), guiLayerConfigs[i]);
					i++;
				}

				m_graphicsAPI = new Vulkan::VulkanAPI();
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
			for (auto& guiLayer : Application::Get().m_guiLayers)
			{
				guiLayer->Init();
			}
		}
		else
			EK_ASSERT(false, "API {0} not initialized!", STRINGIFY(apiType));
	}
}
