#include "precompiled.h"
#include "Viewport.h"

#include "Renderer.h"
#include <Eklipse/Platform/OpenGL/GLViewport.h>
#include <Eklipse/Platform/Vulkan/VKViewport.h>

namespace Eklipse
{
	Ref<Viewport> Viewport::Create(ViewportCreateInfo& info)
	{
		auto apiType = Renderer::GetAPI();
		switch (apiType)
		{
		case ApiType::Vulkan: return CreateRef<Vulkan::VKViewport>(info);
		case ApiType::OpenGL: return CreateRef<OpenGL::GLViewport>(info);
		}
		EK_ASSERT(false, "API {0} not implemented for RenderTarget creation", int(apiType));
		return nullptr;
	}
	void Viewport::Resize(uint32_t width, uint32_t height)
	{
		g_viewportSize = { width, height };
		g_aspectRatio = (float)width / (float)height;
	}
}
