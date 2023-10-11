#include "precompiled.h"
#include "VKViewport.h"
#include "VKBuffers.h"
#include "VKSwapChain.h"
#include "VK.h"
#include "VKCommands.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VKViewport::VKViewport(ViewportCreateInfo& info) : Viewport(info)
		{
			m_framebuffer = CreateRef<VKFramebuffer>(info.framebufferInfo);

			/*if (info.flags & VIEWPORT_BLIT_FRAMEBUFFER)
			{
				FramebufferInfo blitInfo{};
				blitInfo.width = info.framebufferInfo.width;
				blitInfo.height = info.framebufferInfo.height;
				blitInfo.numSamples = 1;
				blitInfo.colorAttachmentInfos = { info.framebufferInfo.colorAttachmentInfos[0] };
				blitInfo.depthAttachmentInfo = { info.framebufferInfo.depthAttachmentInfo };
				m_blitFramebuffer = CreateRef<VKFramebuffer>(blitInfo);
			}*/
		}
		VKViewport::~VKViewport()
		{
		}
		void VKViewport::BindFramebuffer()
		{
			EK_PROFILE();

			m_framebuffer->Bind();
		}
		void VKViewport::UnbindFramebuffer()
		{
			EK_PROFILE();

			//if (m_createInfo.flags & VIEWPORT_BLIT_FRAMEBUFFER)
			//{
			//	VkCommandBuffer commandBuffer = BeginSingleCommands();

			//	int32_t width = m_framebuffer->GetInfo().width;
			//	int32_t height = m_framebuffer->GetInfo().height;
			//	VkImage srcImage = ((VKTexture2D*)m_framebuffer->GetMainColorAttachment())->GetImage();
			//	VkImage dstImage = ((VKTexture2D*)m_blitFramebuffer->GetMainColorAttachment())->GetImage();

			//	VkImageMemoryBarrier imageMemoryBarrier = {};
			//	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			//	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			//	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			//	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			//	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			//	imageMemoryBarrier.image = srcImage;
			//	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			//	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
			//	imageMemoryBarrier.subresourceRange.levelCount = 1;
			//	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
			//	imageMemoryBarrier.subresourceRange.layerCount = 1;
			//	imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			//	imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			//	vkCmdPipelineBarrier(
			//		commandBuffer,
			//		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			//		VK_PIPELINE_STAGE_TRANSFER_BIT,
			//		0,
			//		0, nullptr,
			//		0, nullptr,
			//		1, &imageMemoryBarrier
			//	);

			//	// Transition the destination blit framebuffer image to a transfer destination layout
			//	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			//	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			//	imageMemoryBarrier.image = dstImage;

			//	vkCmdPipelineBarrier(
			//		commandBuffer,
			//		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			//		VK_PIPELINE_STAGE_TRANSFER_BIT,
			//		0,
			//		0, nullptr,
			//		0, nullptr,
			//		1, &imageMemoryBarrier
			//	);

			//	// Perform the image blit operation
			//	VkImageBlit imageBlit = {};
			//	imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			//	imageBlit.srcSubresource.mipLevel = 0;
			//	imageBlit.srcSubresource.baseArrayLayer = 0;
			//	imageBlit.srcSubresource.layerCount = 1;
			//	imageBlit.srcOffsets[0] = { 0, 0, 0 };
			//	imageBlit.srcOffsets[1] = { width, height, 1 };
			//	imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			//	imageBlit.dstSubresource.mipLevel = 0;
			//	imageBlit.dstSubresource.baseArrayLayer = 0;
			//	imageBlit.dstSubresource.layerCount = 1;
			//	imageBlit.dstOffsets[0] = { 0, 0, 0 };
			//	imageBlit.dstOffsets[1] = { static_cast<int32_t>(width), static_cast<int32_t>(height), 1 };

			//	vkCmdBlitImage(
			//		commandBuffer,
			//		srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			//		dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			//		1, &imageBlit,
			//		VK_FILTER_NEAREST
			//	);

			//	// Transition the source framebuffer image back to its original layout
			//	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			//	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			//	imageMemoryBarrier.image = srcImage;
			//	imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			//	imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			//	vkCmdPipelineBarrier(
			//		commandBuffer,
			//		VK_PIPELINE_STAGE_TRANSFER_BIT,
			//		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			//		0,
			//		0, nullptr,
			//		0, nullptr,
			//		1, &imageMemoryBarrier
			//	);

			//	// Transition the destination blit framebuffer image back to its original layout
			//	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			//	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			//	imageMemoryBarrier.image = dstImage;
			//	imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			//	imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			//	vkCmdPipelineBarrier(
			//		commandBuffer,
			//		VK_PIPELINE_STAGE_TRANSFER_BIT,
			//		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			//		0,
			//		0, nullptr,
			//		0, nullptr,
			//		1, &imageMemoryBarrier
			//	);

			//	EndSingleCommands(commandBuffer);
			//}

			m_framebuffer->Unbind();
		}
		void VKViewport::Bind()
		{
			EK_PROFILE();

			Viewport::Bind();
		}
		void VKViewport::Resize(uint32_t width, uint32_t height)
		{
			Viewport::Resize(width, height);
			if (width == 0 || height == 0) return;

			m_framebuffer->Resize(width, height);
			/*if (m_createInfo.flags & VIEWPORT_BLIT_FRAMEBUFFER)
			{
				m_blitFramebuffer->Resize(width, height);
			}*/
		}
	}
}
