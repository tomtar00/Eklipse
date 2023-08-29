#pragma once

#include <glad/glad.h>
#include <Eklipse/Renderer/Framebuffer.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLFramebuffer : public Eklipse::Framebuffer
		{
		public:
			GLFramebuffer(const FramebufferInfo& frambufferInfo);
			virtual ~GLFramebuffer();

			virtual void Bind() override;
			virtual void Unbind() override;
		
		private:
			uint32_t m_id;

			std::vector<uint32_t> m_colorAttachments{};
			uint32_t m_depthAttachment{0};
		};
	}
}