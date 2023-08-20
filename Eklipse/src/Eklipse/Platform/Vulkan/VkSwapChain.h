#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkSwapchainKHR CreateSwapChain(int frameWidth, int frameHeight, uint32_t& minImageCount,
			VkFormat& imageFormat, VkExtent2D& extent, std::vector<VkImage>& images);

		void CreateImages(std::vector<VkImage>& images, std::vector<VmaAllocation>& allocations, int numImages, uint32_t width, uint32_t height, uint32_t mipLevels,
			VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void CreateImageViews(std::vector<VkImageView>& imageViews, std::vector<VkImage>& images, VkFormat format);
		void CreateSamplers(std::vector<VkSampler>& samplers, int numSamplers);
		void CreateFrameBuffers(std::vector<VkFramebuffer>& framebuffers, std::vector<VkImageView>& imageViews, VkRenderPass renderPass, VkExtent2D extent, bool);

		void DestroyImages(std::vector<VkImage>& images, std::vector<VmaAllocation>& allocations);
		void DestroySamplers(std::vector<VkSampler>& samplers);
		void DestroyImageViews(std::vector<VkImageView>& imageViews);
		void DestroyFrameBuffers(std::vector<VkFramebuffer>& buffers);
	}
}