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

		static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		VkImage& Image();
		VkDeviceMemory& Memory();
		VkImageView& ImageView();

		void CreateImage(VkDeviceSize imageSize, uint32_t width, uint32_t height, const void* data);
		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);

	protected:
		void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
		void TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	protected:
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
		void CreateSampler();
		
	private:
		VulkanImage m_image;
		VkSampler m_textureSampler;
	};

	class VulkanDepthImage : public VulkanImage
	{
	public:
		void Init();

		static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
			VkImageTiling tiling, VkFormatFeatureFlags features);
		static VkFormat FindDepthFormat();
		static bool HasStencilComponent(VkFormat format);

	private:
		void CreateDepthImage();
	};
}