#include "precompiled.h"
#include "VKFramebuffer.h"
#include "VK.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VKFramebuffer::VKFramebuffer(const FramebufferInfo& frambufferInfo) : m_framebufferInfo(frambufferInfo)
		{
			Build();
		}
		VKFramebuffer::~VKFramebuffer()
		{
		}
		const FramebufferInfo& VKFramebuffer::GetInfo() const
		{
			return m_framebufferInfo;
		}
		void* VKFramebuffer::GetMainColorAttachment()
		{
			return nullptr;
		}
		void VKFramebuffer::Build()
		{
			std::vector<VkImageView> attachments;

			// Color attachments
			m_colorAttachments.resize(m_framebufferInfo.colorAttachmentInfos.size());
			for (size_t i = 0; i < m_framebufferInfo.colorAttachmentInfos.size(); i++)
			{
				auto& colorAttachmentInfo = m_framebufferInfo.colorAttachmentInfos[i];

				TextureInfo textureInfo = {};
				textureInfo.width = m_framebufferInfo.width;
				textureInfo.height = m_framebufferInfo.height;
				textureInfo.mipMapLevel = 1;
				textureInfo.samples = m_framebufferInfo.numSamples;
				textureInfo.imageFormat = colorAttachmentInfo.textureFormat;
				textureInfo.imageAspect = ImageAspect::COLOR;

				VKTexture2D texture(textureInfo);
				m_colorAttachments[i] = texture;
				attachments.push_back(texture.GetImageView());
			}

			// Depth attachment
			{
				auto& depthAttachmentInfo = m_framebufferInfo.depthAttachmentInfo;

				TextureInfo info = {};
				info.width = m_framebufferInfo.width;
				info.height = m_framebufferInfo.height;
				info.mipMapLevel = 1;
				info.samples = m_framebufferInfo.numSamples;
				info.imageFormat = depthAttachmentInfo.textureFormat;
				info.imageAspect = ImageAspect::DEPTH;

				VKTexture2D texture(info);
				m_depthAttachment = texture;
				attachments.push_back(texture.GetImageView());
			}

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = g_renderPass; // TODO: fix
			framebufferInfo.attachmentCount = m_framebufferInfo.colorAttachmentInfos.size() + 1;
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = g_swapChainExtent.width;
			framebufferInfo.height = g_swapChainExtent.height;
			framebufferInfo.layers = 1;

			VkResult res = vkCreateFramebuffer(g_logicalDevice, &framebufferInfo, nullptr, &m_framebuffer);
			HANDLE_VK_RESULT(res, "CREATE FRAMEBUFFER");
		}
		void VKFramebuffer::Bind()
		{
		}
		void VKFramebuffer::Unbind()
		{
		}
		void VKFramebuffer::Resize(uint32_t width, uint32_t height)
		{
			for (auto& colorAttachment : m_colorAttachments)
				colorAttachment.Destroy();
			m_depthAttachment.Destroy();

			vkDestroyFramebuffer(g_logicalDevice, m_framebuffer, nullptr);

			m_framebufferInfo.width = width;
			m_framebufferInfo.height = height;
			Build();
		}
	}
}