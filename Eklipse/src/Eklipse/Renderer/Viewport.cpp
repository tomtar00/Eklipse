#include "precompiled.h"
#include "Viewport.h"

#include "Renderer.h"
#include <Eklipse/Platform/OpenGL/GLViewport.h>
#include <Eklipse/Platform/Vulkan/VKViewport.h>

namespace Eklipse
{
	Ref<Viewport> Viewport::Create()
	{
		auto apiType = Renderer::GetAPI();
		switch (apiType)
		{
		case ApiType::Vulkan: return CreateRef<Vulkan::VKViewport>();
		case ApiType::OpenGL: return CreateRef<OpenGL::GLViewport>();
		}
		EK_ASSERT(false, "API {0} not implemented for RenderTarget creation", int(apiType));
		return nullptr;
	}
	void Viewport::Draw()
	{
#ifdef EK_EDITOR
		DrawViewport();
#else
		DrawFullscreen();
#endif
	}
}
