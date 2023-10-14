#include "precompiled.h"
#include "GLFramebuffer.h"
#include "GL.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLFramebuffer::GLFramebuffer(const FramebufferInfo& framebufferInfo) : m_id(0), m_framebufferInfo(framebufferInfo)
		{
			if (framebufferInfo.framebufferType & FramebufferType::DEFAULT)
			{
				EK_ASSERT(g_GLDefaultFramebuffer == nullptr, "Default framebuffer already exists!");
				g_defaultFramebuffer = g_GLDefaultFramebuffer = this;
			}
			if (framebufferInfo.framebufferType & FramebufferType::SCENE_VIEW)
			{
				EK_ASSERT(g_GLSceneFramebuffer == nullptr, "Scene framebuffer already exists!");
				g_sceneFramebuffer = g_GLSceneFramebuffer = this;
			}

			Build();
		}
		GLFramebuffer::~GLFramebuffer()
		{
			glDeleteFramebuffers(1, &m_id);
			glDeleteTextures(m_colorAttachments.size(), m_colorAttachments.data());
			glDeleteTextures(1, &m_depthAttachment);
		}
		FramebufferInfo& GLFramebuffer::GetInfo()
		{
			return m_framebufferInfo;
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
					GLenum colorFormat = ConvertToGLFormat(m_framebufferInfo.colorAttachmentInfos[i].textureFormat);

					if (multiSampled)
						glTexImage2DMultisample(m_texTarget, msaaSamples, colorFormat, m_framebufferInfo.width, m_framebufferInfo.height, GL_FALSE);
					else
					{
						glTexImage2D(m_texTarget, 0, colorFormat, m_framebufferInfo.width, m_framebufferInfo.height, 0, colorFormat, GL_UNSIGNED_BYTE, nullptr);

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
			if (m_framebufferInfo.depthAttachmentInfo.textureFormat != ImageFormat::UNDEFINED)
			{
				glGenTextures(1, &m_depthAttachment);
				glBindTexture(m_texTarget, m_depthAttachment);

				GLenum depthFormat = ConvertToGLFormat(m_framebufferInfo.depthAttachmentInfo.textureFormat);

				if (multiSampled)
					glTexImage2DMultisample(m_texTarget, msaaSamples, depthFormat, m_framebufferInfo.width, m_framebufferInfo.height, GL_FALSE);
				else
				{
					glTexStorage2D(m_texTarget, 1, depthFormat, m_framebufferInfo.width, m_framebufferInfo.height);

					glTexParameteri(m_texTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(m_texTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(m_texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_texTarget, m_depthAttachment, 0);
				glBindTexture(m_texTarget, 0);
			}
			else
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
			}

			EK_ASSERT((m_colorAttachments.size() <= 4), "Too many colors attachemnts! ({0})", m_colorAttachments.size());

			auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			EK_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete! Code: {0}", status);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void GLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			glViewport(0, 0, m_framebufferInfo.width, m_framebufferInfo.height);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_colorAttachments.size(), buffers);
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