#include "precompiled.h"
#include "GLFramebuffer.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLFramebuffer::GLFramebuffer(const FramebufferInfo& frambufferInfo) : m_id(0)
		{
			glGenFramebuffers(1, &m_id);
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			// Color attachments
			if (frambufferInfo.colorAttachmentInfos.size() > 0)
			{
				m_colorAttachments.resize(frambufferInfo.colorAttachmentInfos.size());
				glGenTextures(m_colorAttachments.size(), m_colorAttachments.data());

				for (int i = 0; i < m_colorAttachments.size(); i++)
				{
					glBindTexture(GL_TEXTURE_2D, m_colorAttachments[i]);

					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, frambufferInfo.width, frambufferInfo.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorAttachments[i], 0);
				}
			}

			// Depth attachment
			glGenTextures(1, &m_depthAttachment);
			glBindTexture(GL_TEXTURE_2D, m_depthAttachment);

			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, frambufferInfo.width, frambufferInfo.height);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

			EK_ASSERT((m_colorAttachments.size() <= 4), "Too many colors attachemnts! ({0})", m_colorAttachments.size());
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_colorAttachments.size(), buffers);

			EK_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		GLFramebuffer::~GLFramebuffer()
		{
			glDeleteFramebuffers(1, &m_id);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);
		}
		void GLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
			glViewport(0, 0, m_framebufferInfo.width, m_framebufferInfo.height);
		}
		void GLFramebuffer::Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}