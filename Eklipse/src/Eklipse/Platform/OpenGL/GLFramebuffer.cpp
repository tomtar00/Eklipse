#include "precompiled.h"
#include "GLFramebuffer.h"
#include "GL.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLFramebuffer::GLFramebuffer(const FramebufferInfo& framebufferInfo) : m_id(0), m_framebufferInfo(framebufferInfo)
		{
			Build();
		}
		GLFramebuffer::~GLFramebuffer()
		{
			glDeleteFramebuffers(1, &m_id);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);
		}
		const FramebufferInfo& GLFramebuffer::GetInfo() const
		{
			return m_framebufferInfo;
		}
		void* GLFramebuffer::GetMainColorAttachment()
		{
			return &m_colorAttachments[0];
		}
		void GLFramebuffer::Build()
		{
			glGenFramebuffers(1, &m_id);
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			int msaaSamples = m_framebufferInfo.numSamples;
			bool multiSampled = msaaSamples > 1;
			m_texTarget = multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			
			// TODO: maybe use renderbuffers for better performance (no shader read)

			if (m_framebufferInfo.colorAttachmentInfos.size() > 0)
			{
				// Color attachments
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

			auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			EK_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete! Code: {0}", status);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void GLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
		}
		void GLFramebuffer::Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void GLFramebuffer::Resize(uint32_t width, uint32_t height)
		{
			// destroy
			glDeleteFramebuffers(1, &m_id);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);

			// create
			m_framebufferInfo.width = width;
			m_framebufferInfo.height = height;
			Build();
		}
	}
}