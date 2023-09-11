#include "precompiled.h"
#include "GLFramebuffer.h"
#include "GL.h"

#include <Eklipse/Renderer/Settings.h>

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

			/*glDeleteBuffers(1, &m_rectVBO);
			glDeleteVertexArrays(1, &m_rectVAO);*/
		}
		void GLFramebuffer::Build()
		{
			glGenFramebuffers(1, &m_id);
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			int msaaSamples = RendererSettings::GetMsaaSamples();
			bool multiSampled = msaaSamples > 1;
			m_texTarget = multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			
			// TODO: maybe use renderbuffers for better performance (no shader read)

			// Color attachments
			if (m_framebufferInfo.colorAttachmentInfos.size() > 0)
			{
				m_colorAttachments.resize(m_framebufferInfo.colorAttachmentInfos.size());
				glGenTextures(m_colorAttachments.size(), m_colorAttachments.data());

				for (size_t i = 0; i < m_colorAttachments.size(); i++)
				{
					glBindTexture(m_texTarget, m_colorAttachments[i]);

					if (multiSampled)
						glTexImage2DMultisample(m_texTarget, msaaSamples, GL_RGB, m_framebufferInfo.width, m_framebufferInfo.height, GL_FALSE);
					else
					{
						glTexImage2D(m_texTarget, 0, GL_RGB, m_framebufferInfo.width, m_framebufferInfo.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

						glTexParameteri(m_texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(m_texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
						glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					}

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, m_texTarget, m_colorAttachments[i], 0);
				}
				glBindTexture(m_texTarget, 0);
			}

			// TODO: change
			g_viewportTexture = m_colorAttachments[0];

			// Depth and stencil attachment
			glGenTextures(1, &m_depthAttachment);
			glBindTexture(m_texTarget, m_depthAttachment);

			if (multiSampled)
				glTexImage2DMultisample(m_texTarget, msaaSamples, GL_DEPTH24_STENCIL8, m_framebufferInfo.width, m_framebufferInfo.height, GL_FALSE);
			else
			{
				glTexStorage2D(m_texTarget, 1, GL_DEPTH24_STENCIL8, m_framebufferInfo.width, m_framebufferInfo.height);

				glTexParameteri(m_texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(m_texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_texTarget, m_depthAttachment, 0);
			glBindTexture(m_texTarget, 0);

			EK_ASSERT((m_colorAttachments.size() <= 4), "Too many colors attachemnts! ({0})", m_colorAttachments.size());
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_colorAttachments.size(), buffers);

			EK_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE), "Framebuffer is incomplete! Code: {0}", glCheckFramebufferStatus(GL_FRAMEBUFFER));
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Rect
			 //float rectangleVertices[] =
			 //{
			 //	// Coords      // texCoords
			 //	 1.0f, -1.0f,  1.0f, 0.0f,
			 //	-1.0f, -1.0f,  0.0f, 0.0f,
			 //	-1.0f,  1.0f,  0.0f, 1.0f,
			 //  
			 //	 1.0f,  1.0f,  1.0f, 1.0f,
			 //	 1.0f, -1.0f,  1.0f, 0.0f,
			 //	-1.0f,  1.0f,  0.0f, 1.0f
			 //};
			 //glGenVertexArrays(1, &m_rectVAO);
			 //glGenBuffers(1, &m_rectVBO);
			 //glBindVertexArray(m_rectVAO);
			 //glBindBuffer(GL_ARRAY_BUFFER, m_rectVBO);
			 //glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
			 //glEnableVertexAttribArray(0);
			 //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			 //glEnableVertexAttribArray(1);
			 //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		}
		void GLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
			glViewport(0, 0, m_framebufferInfo.width, m_framebufferInfo.height);
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
			glBindTexture(m_texTarget, m_colorAttachments[0]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		void GLFramebuffer::Resize(uint32_t width, uint32_t height)
		{
			// destroy
			glDeleteFramebuffers(1, &m_id);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);

			/*glDeleteBuffers(1, &m_rectVBO);
			glDeleteVertexArrays(1, &m_rectVAO);*/

			// create
			m_framebufferInfo.width = width;
			m_framebufferInfo.height = height;
			Build();
		}
	}
}