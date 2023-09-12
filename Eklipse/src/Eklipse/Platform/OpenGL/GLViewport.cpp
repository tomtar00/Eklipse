#include "precompiled.h"
#include "GLViewport.h"
#include <glad/glad.h>

namespace Eklipse
{
	namespace OpenGL
	{
		GLViewport::GLViewport(ViewportCreateInfo& info) : Viewport(info)
		{
			m_framebuffer = Framebuffer::Create(info.framebufferInfo);

			if (info.flags & ViewportFlags::VIEWPORT_BLIT_FRAMEBUFFER)
			{
				FramebufferInfo blitInfo{};
				blitInfo.width = info.framebufferInfo.width;
				blitInfo.height = info.framebufferInfo.height;
				blitInfo.colorAttachmentInfos = { info.framebufferInfo.colorAttachmentInfos[0] };
				m_blitFramebuffer = CreateRef<GLFramebuffer>(blitInfo);
			}
		}
		void GLViewport::Bind()
		{
			m_framebuffer->Bind();

			if (m_createInfo.flags & ViewportFlags::VIEWPORT_BLIT_FRAMEBUFFER)
			{
				m_blitFramebuffer->Bind();
			}
		}
		void GLViewport::Unbind()
		{
			if (m_createInfo.flags & ViewportFlags::VIEWPORT_BLIT_FRAMEBUFFER)
			{
				uint32_t width = m_framebuffer->GetInfo().width;
				uint32_t height = m_framebuffer->GetInfo().height;
				glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			}

			m_framebuffer->Unbind();
		}
		void GLViewport::Resize(uint32_t width, uint32_t height)
		{
			m_framebuffer->Resize(width, height);

			if (m_createInfo.flags & ViewportFlags::VIEWPORT_BLIT_FRAMEBUFFER)
			{
				m_blitFramebuffer->Resize(width, height);
			}
		}
		void GLViewport::DrawViewport()
		{

		}
		void GLViewport::DrawFullscreen()
		{
		}
	}
}
