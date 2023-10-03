#pragma once
#include <Eklipse/Renderer/Framebuffer.h>
#include <vulkan/vulkan.h>
#include "VKTexture.h"

namespace Eklipse
{
	namespace Vulkan
	{
		class VKFramebuffer : public Eklipse::Framebuffer
		{
		public:
			VKFramebuffer(const FramebufferInfo& frambufferInfo);
			virtual ~VKFramebuffer();

			virtual const FramebufferInfo& GetInfo() const override;
			virtual void* GetMainColorAttachment() override;

			virtual void Build() override;
			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Resize(uint32_t width, uint32_t height) override;

		private:
			FramebufferInfo m_framebufferInfo;
			VkFramebuffer m_framebuffer;
			std::vector<VKTexture2D> m_colorAttachments{};
			VKTexture2D m_depthAttachment;
		};
	}
}