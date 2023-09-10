#include "precompiled.h"
#include "GLFramebuffer.h"
#include "GL.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLFramebuffer* g_framebuffer;

		GLFramebuffer::GLFramebuffer(const FramebufferInfo& framebufferInfo) : m_id(0)
		{
			g_framebuffer = this;
			m_framebufferInfo = framebufferInfo;

			Build();
		}
		GLFramebuffer::~GLFramebuffer()
		{
			glDeleteFramebuffers(1, &m_id);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);
			//glDeleteBuffers(1, &m_rectVBO);
			//glDeleteVertexArrays(1, &m_rectVAO);
		}
		void GLFramebuffer::Build()
		{
			glGenFramebuffers(1, &m_id);
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			// TODO: handle multisampling and texture formats
			// TODO: maybe use renderbuffers for better performance

			// Color attachments
			if (m_framebufferInfo.colorAttachmentInfos.size() > 0)
			{
				m_colorAttachments.resize(m_framebufferInfo.colorAttachmentInfos.size());
				glGenTextures(m_colorAttachments.size(), m_colorAttachments.data());

				for (int i = 0; i < m_colorAttachments.size(); i++)
				{
					glBindTexture(GL_TEXTURE_2D, m_colorAttachments[i]);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_framebufferInfo.width, m_framebufferInfo.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorAttachments[i], 0);
				}
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			// TODO: change
			g_viewportTexture = m_colorAttachments[0];

			// Depth and stencil attachment
			glGenTextures(1, &m_depthAttachment);
			glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_framebufferInfo.width, m_framebufferInfo.height);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			EK_ASSERT((m_colorAttachments.size() <= 4), "Too many colors attachemnts! ({0})", m_colorAttachments.size());
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_colorAttachments.size(), buffers);

			EK_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Framebuffer is incomplete! Code: {0}", glCheckFramebufferStatus(GL_FRAMEBUFFER));
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Rect
			// float rectangleVertices[] =
			// {
			// 	// Coords      // texCoords
			// 	 1.0f, -1.0f,  1.0f, 0.0f,
			// 	-1.0f, -1.0f,  0.0f, 0.0f,
			// 	-1.0f,  1.0f,  0.0f, 1.0f,
			   
			// 	 1.0f,  1.0f,  1.0f, 1.0f,
			// 	 1.0f, -1.0f,  1.0f, 0.0f,
			// 	-1.0f,  1.0f,  0.0f, 1.0f
			// };
			// glGenVertexArrays(1, &m_rectVAO);
			// glGenBuffers(1, &m_rectVBO);
			// glBindVertexArray(m_rectVAO);
			// glBindBuffer(GL_ARRAY_BUFFER, m_rectVBO);
			// glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
			// glEnableVertexAttribArray(0);
			// glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			// glEnableVertexAttribArray(1);
			// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		}
		void GLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
		}
		void GLFramebuffer::Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void GLFramebuffer::Draw()
		{
			glBindVertexArray(m_rectVAO);
			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, m_colorAttachments[0]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		void GLFramebuffer::Resize(uint32_t width, uint32_t height)
		{
			// destroy
			glDeleteFramebuffers(1, &m_id);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);
			//glDeleteBuffers(1, &m_rectVBO);
			//glDeleteVertexArrays(1, &m_rectVAO);

			// create
			m_framebufferInfo.width = width;
			m_framebufferInfo.height = height;
			Build();
		}
	}
}