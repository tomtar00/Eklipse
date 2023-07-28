#include "precompiled.h"
#include "Renderer.h"

#include <Eklipse/Platform/Vulkan/VulkanAPI.h>

namespace Eklipse
{
	Renderer::Renderer() : m_graphicsAPI(nullptr) {}
	Renderer::~Renderer()
	{
		delete m_graphicsAPI;
	}

	void Renderer::Update()
	{
		m_graphicsAPI->DrawFrame();
	}

	void Renderer::PostMainLoop()
	{
		m_graphicsAPI->WaitIdle();
	}

	ApiType Renderer::GetAPI()
	{
		return m_graphicsAPI->GetApiType();
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
			if (apiType == m_graphicsAPI->GetApiType() && m_graphicsAPI->IsInitialized())
			{
				EK_CORE_WARN("{0} API already set and initialized!", (int)apiType);
				return;
			}

			// shutdown old api
			if (m_graphicsAPI->IsInitialized())
			{
				m_graphicsAPI->Shutdown();
				delete m_graphicsAPI;

				EK_ASSERT((m_graphicsAPI == nullptr),
					"When switching api, old api ({0}) was not deleted", 
					(int)m_graphicsAPI->GetApiType());
			}
		}		

		// init new api
		switch (apiType)
		{
			case ApiType::Vulkan:
			{
				m_graphicsAPI = new VulkanAPI();				
				break;
			}
			default:
			{
				EK_ASSERT(false, "API {0} not implemented!", (int)apiType);
				break;
			}
		}
		m_graphicsAPI->SetApiType(apiType);

		if (!m_graphicsAPI->IsInitialized())
			m_graphicsAPI->Init();
		else
			EK_ASSERT(false, "API {0} not initialized!", (int)apiType);
	}
}
