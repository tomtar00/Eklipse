#include "precompiled.h"
#include "VKViewport.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VKViewport::VKViewport(ViewportCreateInfo& info) : Viewport(info)
		{
		
		}
		VKViewport::~VKViewport()
		{
		}
		void VKViewport::BindFramebuffer()
		{
		}
		void VKViewport::UnbindFramebuffer()
		{
		}
		void VKViewport::Bind()
		{
		}
		Ref<VertexArray> VKViewport::GetVertexArray() const
		{
			return Ref<VertexArray>();
		}
		void VKViewport::Resize(uint32_t width, uint32_t height)
		{
			Viewport::Resize(width, height);
			if (width == 0 || height == 0) return;
		}
	}
}
