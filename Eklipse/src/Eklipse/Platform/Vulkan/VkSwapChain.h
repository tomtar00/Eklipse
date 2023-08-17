#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkSwapchainKHR CreateSwapChain(int frameWidth, int frameHeight, uint32_t& minImageCount,
			VkFormat& imageFormat, VkExtent2D& extent, std::vector<VkImage>& images);

		void CreateSwapChainImageViews(std::vector<VkImageView>& imageViews, std::vector<VkImage>& images);
		void CreateFrameBuffers(std::vector<VkFramebuffer>& framebuffers, std::vector<VkImageView>& imageViews, VkRenderPass renderPass, VkExtent2D extent);

		void DestroyFrameBuffers(std::vector<VkFramebuffer>& buffers);
		void DestroyImageViews(std::vector<VkImageView>& imageViews);
	}
}