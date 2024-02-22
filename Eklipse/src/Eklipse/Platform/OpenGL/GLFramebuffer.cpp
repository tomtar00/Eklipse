#include "precompiled.h"
#include "GLFramebuffer.h"
#include "GL.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLFramebuffer::GLFramebuffer(const FramebufferInfo& framebufferInfo) 
			: Framebuffer(framebufferInfo), m_id(0), m_framebufferInfo(framebufferInfo)
		{
			EK_CORE_PROFILE();
			m_aspectRatio = (float)framebufferInfo.width / (float)framebufferInfo.height;

			if (framebufferInfo.isDefaultFramebuffer)
			{
				EK_ASSERT(g_GLDefaultFramebuffer == nullptr, "Default framebuffer already exists!");
				g_defaultFramebuffer = g_GLDefaultFramebuffer = this;
			}
			else
			{
				g_offScreenFramebuffers.push_back(this);
				g_GLOffScreenFramebuffers.push_back(this);
			}

			Build();
		}

		FramebufferInfo& GLFramebuffer::GetInfo()
		{
			return m_framebufferInfo;
		}
		uint32_t GLFramebuffer::GetMainColorAttachment()
		{
			return m_colorAttachments[0]->GetID();
		}

		void GLFramebuffer::Build()
		{
			EK_CORE_PROFILE();

			glGenFramebuffers(1, &m_id);
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			int msaaSamples = m_framebufferInfo.numSamples;
			bool multiSampled = msaaSamples > 1;
			m_texTarget = multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

			if (m_framebufferInfo.colorAttachmentInfos.size() > 0)
			{
				// Color attachments
				m_colorAttachments.resize(m_framebufferInfo.colorAttachmentInfos.size());
				for (size_t i = 0; i < m_colorAttachments.size(); i++)
				{
					TextureInfo textureInfo{};
					textureInfo.width = m_framebufferInfo.width;
					textureInfo.height = m_framebufferInfo.height;
					textureInfo.mipMapLevel = 1;
					textureInfo.samples = m_framebufferInfo.numSamples;
					textureInfo.imageFormat = m_framebufferInfo.colorAttachmentInfos[i].textureFormat;
					textureInfo.imageLayout = ImageLayout::SHADER_READ_ONLY;
					textureInfo.imageAspect = ImageAspect::COLOR;
					textureInfo.imageUsage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED;

					uint32_t singlePixelSize = FormatToChannels(textureInfo.imageFormat);
					uint32_t dataSize = m_framebufferInfo.width * m_framebufferInfo.height * singlePixelSize;
					TextureData data{};
					data.info = textureInfo;
					data.data = nullptr;
					data.size = dataSize;

					m_colorAttachments[i] = CreateRef<GLTexture2D>(data);

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, m_texTarget, m_colorAttachments[i]->GetID(), 0);
				}
				glBindTexture(m_texTarget, 0);
			}

			// Depth and stencil attachment
			if (m_framebufferInfo.depthAttachmentInfo.textureFormat != ImageFormat::FORMAT_UNDEFINED)
			{
				TextureInfo textureInfo{};
				textureInfo.width = m_framebufferInfo.width;
				textureInfo.height = m_framebufferInfo.height;
				textureInfo.mipMapLevel = 1;
				textureInfo.samples = m_framebufferInfo.numSamples;
				textureInfo.imageFormat = m_framebufferInfo.depthAttachmentInfo.textureFormat;
				textureInfo.imageAspect = ImageAspect::DEPTH;
				textureInfo.imageUsage = ImageUsage::DEPTH_ATTACHMENT;

				uint32_t singlePixelSize = FormatToChannels(textureInfo.imageFormat);
				uint32_t dataSize = m_framebufferInfo.width * m_framebufferInfo.height * singlePixelSize;
				TextureData data{};
				data.info = textureInfo;
				data.data = nullptr;
				data.size = dataSize;

				m_depthAttachment = CreateRef<GLTexture2D>(data);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_texTarget, m_depthAttachment->GetID(), 0);
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
			EK_CORE_PROFILE();

			glBindFramebuffer(GL_FRAMEBUFFER, m_id);

			glViewport(0, 0, m_framebufferInfo.width, m_framebufferInfo.height);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_colorAttachments.size(), buffers);

			g_currentFramebuffer = this;
		}
		void GLFramebuffer::Unbind()
		{
			EK_CORE_PROFILE();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			g_currentFramebuffer = nullptr;
		}
		void GLFramebuffer::Resize(uint32_t width, uint32_t height)
		{
			EK_CORE_PROFILE();
			Framebuffer::Resize(width, height);

			// destroy
			Dispose();

			// create
			m_framebufferInfo.width = width;
			m_framebufferInfo.height = height;
			Build();
		}
		Ref<Texture2D> GLFramebuffer::GetColorAttachment(uint32_t index)
		{
			return m_colorAttachments[index];
		}
		void GLFramebuffer::Dispose()
		{
			EK_CORE_PROFILE();
			glDeleteFramebuffers(1, &m_id);
			
			for (auto& colorAttachment : m_colorAttachments)
                colorAttachment->Dispose();

			if (m_depthAttachment)
				m_depthAttachment->Dispose();
		}
	}
}