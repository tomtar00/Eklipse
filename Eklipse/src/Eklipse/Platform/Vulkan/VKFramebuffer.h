#pragma once

#include <Eklipse/Renderer/Framebuffer.h>

namespace Eklipse
{
	namespace Vulkan
	{
		class VKFramebuffer : public Eklipse::Framebuffer
		{
		public:
			VKFramebuffer(const FramebufferInfo& frambufferInfo);
			virtual ~VKFramebuffer();

			virtual void* GetMainColorAttachment() override;

			virtual void Build() override;
			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Resize(uint32_t width, uint32_t height) override;
		};
	}
}