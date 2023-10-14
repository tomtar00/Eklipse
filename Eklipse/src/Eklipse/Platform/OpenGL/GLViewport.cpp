#include "precompiled.h"
//
//#include "GL.h"
//#include "GLViewport.h"
//#include <glad/glad.h>
//
//namespace Eklipse
//{
//	namespace OpenGL
//	{
//		GLViewport::GLViewport(ViewportCreateInfo& info) : Viewport(info)
//		{
//			m_framebuffer = CreateRef<GLFramebuffer>(info.framebufferInfo);
//			g_viewportTexture = m_framebuffer->GetMainColorAttachment();
//
//			if (info.flags & VIEWPORT_BLIT_FRAMEBUFFER)
//			{
//				FramebufferInfo blitInfo{};
//				blitInfo.width = info.framebufferInfo.width;
//				blitInfo.height = info.framebufferInfo.height;
//				blitInfo.numSamples = 1;
//				blitInfo.colorAttachmentInfos = { info.framebufferInfo.colorAttachmentInfos[0] };
//				blitInfo.depthAttachmentInfo = { info.framebufferInfo.depthAttachmentInfo };
//				m_blitFramebuffer = CreateRef<GLFramebuffer>(blitInfo);
//
//				g_viewportTexture = m_blitFramebuffer->GetMainColorAttachment();				
//			}
//		}
//		void GLViewport::BindFramebuffer()
//		{
//			EK_PROFILE();
//
//			m_framebuffer->Bind();
//
//			glViewport(0, 0, m_framebuffer->GetInfo().width, m_framebuffer->GetInfo().height);
//			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//			glEnable(GL_DEPTH_TEST);
//		}
//		void GLViewport::UnbindFramebuffer()
//		{
//			EK_PROFILE();
//
//			if (m_createInfo.flags & VIEWPORT_BLIT_FRAMEBUFFER)
//			{
//				glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer->m_id);
//				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_blitFramebuffer->m_id);
//
//				uint32_t width = m_framebuffer->GetInfo().width;
//				uint32_t height = m_framebuffer->GetInfo().height;
//				glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//			}
//
//			m_framebuffer->Unbind();
//		}
//		void GLViewport::Resize(uint32_t width, uint32_t height)
//		{
//			Viewport::Resize(width, height);
//			if (width == 0 || height == 0) return;
//
//			m_framebuffer->Resize(width, height);
//			if (m_createInfo.flags & VIEWPORT_BLIT_FRAMEBUFFER)
//			{
//				m_blitFramebuffer->Resize(width, height);
//			}
//		}
//		void GLViewport::Bind()
//		{
//			EK_PROFILE();
//
//			if (m_createInfo.flags & VIEWPORT_FULLSCREEN)
//			{
//				m_vertexArray->Bind();
//				glDisable(GL_DEPTH_TEST);
//				glBindTexture(GL_TEXTURE_2D, g_viewportTexture);
//			}
//		}
//	}
//}
