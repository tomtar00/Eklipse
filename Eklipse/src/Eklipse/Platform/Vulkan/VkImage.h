#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Texture.h>

#include <vk_mem_alloc.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkImageView ICreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

		class Image
		{
		public:
			void Image::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels,
				VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
				VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
			void Dispose();

			VkImage m_image;
			VkImageView m_imageView;
			VmaAllocation m_allocation;
			VkSampler m_sampler;

		protected:
			void AllocateOnGPU(VkDeviceSize imageSize, uint32_t width, uint32_t height, uint32_t mipLevels, const void* data);
			void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
			void CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
			void TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		};

		class DepthImage : public Image 
		{
		public:
			void Setup(VkSampleCountFlagBits numSamples);
		};
		class ColorImage : public Image 
		{
		public:
			void Setup(VkSampleCountFlagBits numSamples);
		};

		class Texture : public Image
		{
		public:
			void Load(TextureData data);

		private:
			void CreateSampler();
			void GenerateMipMaps(uint32_t mipLevels, uint32_t width, uint32_t height);

			uint32_t m_mipLevels;
		};
	}
}