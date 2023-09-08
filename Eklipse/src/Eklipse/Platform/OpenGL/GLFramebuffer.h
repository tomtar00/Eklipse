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

			virtual void Build() override;
			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Draw() override;
			virtual void Resize(uint32_t width, uint32_t height) override;
		
		private:
			uint32_t m_id;
			uint32_t m_rectVAO;
			uint32_t m_rectVBO;

			std::vector<uint32_t> m_colorAttachments{};
			uint32_t m_depthAttachment{0};
		};
	}
}