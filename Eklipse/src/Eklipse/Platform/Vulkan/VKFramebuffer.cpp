#include "precompiled.h"
#include "VKFramebuffer.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VKFramebuffer::VKFramebuffer(const FramebufferInfo& frambufferInfo) : m_framebufferInfo(frambufferInfo)
		{

		}
		VKFramebuffer::~VKFramebuffer()
		{
		}
		const FramebufferInfo& VKFramebuffer::GetInfo() const
		{
			return m_framebufferInfo;
		}
		void* VKFramebuffer::GetMainColorAttachment()
		{
			return nullptr;
		}
		void VKFramebuffer::Build()
		{
		}
		void VKFramebuffer::Bind()
		{
		}
		void VKFramebuffer::Unbind()
		{
		}
		void VKFramebuffer::Resize(uint32_t width, uint32_t height)
		{
		}
	}
}