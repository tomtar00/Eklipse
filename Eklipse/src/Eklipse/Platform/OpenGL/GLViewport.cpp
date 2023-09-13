#include "precompiled.h"

#include "GL.h"
#include "GLViewport.h"
#include <glad/glad.h>

namespace Eklipse
{
	namespace OpenGL
	{
		GLViewport::GLViewport(ViewportCreateInfo& info) : Viewport(info)
		{
			m_framebuffer = Framebuffer::Create(info.framebufferInfo);
			g_viewportTexture = *(uint32_t*)m_framebuffer->GetMainColorAttachment();

			if (info.flags & ViewportFlags::VIEWPORT_BLIT_FRAMEBUFFER)
			{
				FramebufferInfo blitInfo{};
				blitInfo.width = info.framebufferInfo.width;
				blitInfo.height = info.framebufferInfo.height;
				blitInfo.numSamples = 1;
				blitInfo.colorAttachmentInfos = { info.framebufferInfo.colorAttachmentInfos[0] };
				blitInfo.depthAttachmentInfo = { info.framebufferInfo.depthAttachmentInfo };
				m_blitFramebuffer = CreateRef<GLFramebuffer>(blitInfo);

				g_viewportTexture = *(uint32_t*)m_blitFramebuffer->GetMainColorAttachment();

				// Rect
				float rectangleVertices[] =
				{
				   // Vert Coords // Tex Coords
					 1.0f, -1.0f,  1.0f, 0.0f,
					-1.0f, -1.0f,  0.0f, 0.0f,
					-1.0f,  1.0f,  0.0f, 1.0f,
				  
					 1.0f,  1.0f,  1.0f, 1.0f,
					 1.0f, -1.0f,  1.0f, 0.0f,
					-1.0f,  1.0f,  0.0f, 1.0f
				};
				glGenVertexArrays(1, &m_rectVAO);
				glGenBuffers(1, &m_rectVBO);
				glBindVertexArray(m_rectVAO);
				glBindBuffer(GL_ARRAY_BUFFER, m_rectVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			}
		}
		GLViewport::~GLViewport()
		{
			glDeleteBuffers(1, &m_rectVBO);
			glDeleteVertexArrays(1, &m_rectVAO);
		}
		void GLViewport::Bind()
		{
			m_framebuffer->Bind();

			if (m_createInfo.flags & ViewportFlags::VIEWPORT_BLIT_FRAMEBUFFER)
			{
				m_blitFramebuffer->Bind();
			}

			glViewport(0, 0, m_framebuffer->GetInfo().width, m_framebuffer->GetInfo().height);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
		}
		void GLViewport::Unbind()
		{
			if (m_createInfo.flags & ViewportFlags::VIEWPORT_BLIT_FRAMEBUFFER)
			{
				uint32_t width = m_framebuffer->GetInfo().width;
				uint32_t height = m_framebuffer->GetInfo().height;
				EK_TEST("{0} {1}", width, height);
				glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			}

			m_framebuffer->Unbind();
		}
		void GLViewport::Resize(uint32_t width, uint32_t height)
		{
			m_framebuffer->Resize(width, height);

			if (m_createInfo.flags & ViewportFlags::VIEWPORT_BLIT_FRAMEBUFFER)
			{
				glDeleteBuffers(1, &m_rectVBO);
				glDeleteVertexArrays(1, &m_rectVAO);

				m_blitFramebuffer->Resize(width, height);
			}
		}
		void GLViewport::DrawViewport()
		{

		}
		void GLViewport::DrawFullscreen()
		{
			glBindVertexArray(m_rectVAO);
			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, g_viewportTexture);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}
