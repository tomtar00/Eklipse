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
			Vec<Ref<VKTexture2D>> colorAttachments{};
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

			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual Ref<Texture2D> GetColorAttachment(uint32_t index) override;

			virtual FramebufferInfo& GetInfo() override;
			virtual void Build() override;
			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Dispose() override;

		private:
			VkRenderPass CreateRenderPass();

		private:
			FramebufferInfo m_framebufferInfo;
			VkRenderPass m_renderPass;
			Vec<VkFramebuffer> m_framebuffers;
			Vec<VkFramebufferAttachments> m_framebufferAttachments;
			Vec<VkCommandBuffer> m_commandBuffers;
			uint32_t m_imageIndex = 0;
		};
	}
}