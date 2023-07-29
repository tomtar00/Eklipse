#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanImage
	{
	public:
		void Init(uint32_t width, uint32_t height, 
			VkFormat format, VkImageTiling tiling, 
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties
		);
		void Shutdown();

		void TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
		
		void CreateImageView(VkFormat format);
		static VkImageView CreateImageView(VkImage image, VkFormat format);

		VkImage& Image();
		VkDeviceMemory& Memory();
		VkImageView& ImageView();

	private:
		VkImage m_image;
		VkDeviceMemory m_imageMemory;
		VkImageView m_imageView;
	};

	class VulkanTexture
	{
	public:
		void Load(char* path);
		void Shutdown();

		VulkanImage& Image();
		VkSampler& Sampler();

	private:
		void CreateImage(VkDeviceSize imageSize, uint32_t width, uint32_t height, const void* data);
		void CreateSampler();
		
	private:
		VulkanImage m_image;
		VkSampler m_textureSampler;
	};
}