#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkSwapchainKHR CreateSwapChain(int frameWidth, int frameHeight, uint32_t& minImageCount,
			VkFormat& imageFormat, VkPresentModeKHR& presentMode, VkExtent2D& extent, Vec<VkImage>& images);

		void CreateImages(Vec<VkImage>& images, Vec<VmaAllocation>& allocations, int numImages, uint32_t width, uint32_t height, uint32_t mipLevels,
			VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
		void CreateImageViews(Vec<VkImageView>& imageViews, Vec<VkImage>& images, VkFormat format);
		void CreateSamplers(Vec<VkSampler>& samplers, int numSamplers);
		void CreateFrameBuffers(Vec<VkFramebuffer>& framebuffers, Vec<VkImageView>& imageViews, VkRenderPass renderPass, VkExtent2D extent, bool);

		void DestroyImages(Vec<VkImage>& images, Vec<VmaAllocation>& allocations);
		void DestroySamplers(Vec<VkSampler>& samplers);
		void DestroyImageViews(Vec<VkImageView>& imageViews);
		void DestroyFrameBuffers(Vec<VkFramebuffer>& buffers);
	}
}