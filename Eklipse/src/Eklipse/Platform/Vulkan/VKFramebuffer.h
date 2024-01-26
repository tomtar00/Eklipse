#pragma once
#include <Eklipse/Renderer/Framebuffer.h>
#include <vulkan/vulkan.h>
#include "VKTexture.h"

namespace Eklipse
{
	namespace Vulkan
	{
		struct VkFramebufferAttachments
		{
			std::vector<Ref<VKTexture2D>> colorAttachments{};
			Ref<VKTexture2D> depthAttachment;
		};

		class VKFramebuffer : public Framebuffer
		{
		public:
			VKFramebuffer(const FramebufferInfo& framebufferInfo);

			void DestroyFramebuffers();

			VKTexture2D& GetMainColorAttachment(uint32_t index);
			VkRenderPass GetRenderPass() const;
			VkCommandBuffer GetCommandBuffer(uint32_t index);
			uint32_t* GetImageIndexPtr();

			virtual FramebufferInfo& GetInfo() override;
			virtual void Build() override;
			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void Dispose() override;

		private:
			VkRenderPass CreateRenderPass();

		private:
			FramebufferInfo m_framebufferInfo;
			VkRenderPass m_renderPass;
			std::vector<VkFramebuffer> m_framebuffers;
			std::vector<VkFramebufferAttachments> m_framebufferAttachments;
			std::vector<VkCommandBuffer> m_commandBuffers;
			uint32_t m_imageIndex = 0;
		};
	}
}