#pragma once

#include <glad/glad.h>
#include <Eklipse/Renderer/Framebuffer.h>

namespace Eklipse
{
	namespace OpenGL
	{
		static GLenum ConvertToGLFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::UNDEFINED:	return GL_NONE;
				case ImageFormat::R8:			return GL_RED;
				case ImageFormat::RGB8:			return GL_RGB;
				case ImageFormat::RGBA8:		return GL_RGBA;
				case ImageFormat::BGRA8:		return GL_BGRA;
				case ImageFormat::D24S8:		return GL_DEPTH24_STENCIL8;
			}

			EK_ASSERT(false, "Wrong image format");
			return 0;
		}

		class GLFramebuffer : public Eklipse::Framebuffer
		{
		public:
			GLFramebuffer(const FramebufferInfo& frambufferInfo);
			virtual ~GLFramebuffer() = default;

			virtual FramebufferInfo& GetInfo() override;
			uint32_t GetMainColorAttachment() { return m_colorAttachments[0]; }

			virtual void Build() override;
			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void Dispose() override;
		
			uint32_t m_id;
		private:
			FramebufferInfo m_framebufferInfo;
			GLenum m_texTarget;

			std::vector<uint32_t> m_colorAttachments{};
			uint32_t m_depthAttachment{0};
		};
	}
}