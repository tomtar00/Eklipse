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
			std::vector<VKTexture2D> colorAttachments{};
			VKTexture2D depthAttachment;
		};

		class VKFramebuffer : public Eklipse::Framebuffer
		{
		public:
			VKFramebuffer(const FramebufferInfo& framebufferInfo);
			virtual ~VKFramebuffer() = default;

			void DestroyFramebuffers();

			virtual FramebufferInfo& GetInfo() { return m_framebufferInfo; }
			inline VKTexture2D GetMainColorAttachment(uint32_t index) { return m_framebufferAttachments[index].colorAttachments[0]; }
			inline VkRenderPass GetRenderPass() { return m_renderPass; }
			inline VkCommandBuffer GetCommandBuffer(uint32_t index) { return m_commandBuffers[index]; }

			virtual void Build() override;
			virtual void Bind() override;
			virtual void Unbind() override;
			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual void Dispose() override;

		private:
			VkRenderPass CreateRenderPass();

		private:
			FramebufferInfo m_framebufferInfo;
			std::vector<VkFramebuffer> m_framebuffers;
			std::vector<VkFramebufferAttachments> m_framebufferAttachments;
			VkRenderPass m_renderPass;
			std::vector<VkCommandBuffer> m_commandBuffers;
		};
	}
}