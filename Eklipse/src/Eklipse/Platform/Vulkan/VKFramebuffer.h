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

			virtual void Bind() override;
			virtual void Unbind() override;
		};
	}
}