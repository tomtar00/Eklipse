#pragma once

#include <glad/glad.h>
#include <Eklipse/Renderer/Framebuffer.h>
#include <Eklipse/Platform/OpenGL/GLTexture.h>

namespace Eklipse
{
	namespace OpenGL
	{
		static GLenum ConvertToGLFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::FORMAT_UNDEFINED:	return GL_NONE;
				case ImageFormat::R8:				return GL_RED;
				case ImageFormat::RGB8:				return GL_RGB;
				case ImageFormat::RGBA8:			return GL_RGBA;
				case ImageFormat::BGRA8:			return GL_BGRA;
				case ImageFormat::RGBA32F:			return GL_RGBA32F;
				case ImageFormat::D24S8:			return GL_DEPTH24_STENCIL8;
			}

			EK_ASSERT(false, "Wrong image format");
			return 0;
		}

		class GLFramebuffer : public Framebuffer
		{
		public:
			GLFramebuffer(const FramebufferInfo& frambufferInfo);

			virtual FramebufferInfo& GetInfo() override;
			uint32_t GetMainColorAttachment();

			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual Ref<Texture2D> GetColorAttachment(uint32_t index) override;

			virtual void Build() override;
			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Dispose() override;
		
		private:
			uint32_t m_id;
			FramebufferInfo m_framebufferInfo;
			GLenum m_texTarget;

			Vec<Ref<GLTexture2D>> m_colorAttachments{};
			Ref<GLTexture2D> m_depthAttachment{};
		};
	}
}