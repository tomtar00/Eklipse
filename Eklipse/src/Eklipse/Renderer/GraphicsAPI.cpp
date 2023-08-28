#include "precompiled.h"
#include "GraphicsAPI.h"

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VulkanAPI.h>
#include <Eklipse/Platform/OpenGL/OpenGLAPI.h>

namespace Eklipse
{
	GraphicsAPI::GraphicsAPI() :
		m_initialized(false),
		m_scene(nullptr) {}

	Scene* GraphicsAPI::GetScene()
	{
		return m_scene;
	}

	bool GraphicsAPI::IsInitialized()
	{
		return m_initialized;
	}
	Ref<GraphicsAPI> GraphicsAPI::Create()
	{
		auto apiType = Renderer::GetAPI();
		switch (apiType)
		{
			case ApiType::Vulkan: return CreateRef<Vulkan::VulkanAPI>();
			case ApiType::OpenGL: return CreateRef<OpenGL::OpenGLAPI>();
		}
		EK_ASSERT(false, "API {0} not implemented!", (int)apiType);
		return nullptr;
	}
}