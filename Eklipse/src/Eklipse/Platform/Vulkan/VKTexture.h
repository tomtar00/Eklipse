#pragma once
#include <Eklipse/Renderer/Texture.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Eklipse
{
	namespace Vulkan
	{
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		VkSampler CreateSampler(float mipLevels);
		VkImage CreateImage(VmaAllocation* allocation, uint32_t width, uint32_t height, uint32_t mipLevels,
			VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage);

		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void GenerateMipMaps(VkImage image, uint32_t mipLevels, uint32_t width, uint32_t height);

		class VKTexture2D : public Eklipse::Texture2D
		{
		public:
			VKTexture2D() = default;
			VKTexture2D(const TextureInfo& textureInfo);
			virtual ~VKTexture2D() = default;

			virtual void SetData(void* data, uint32_t size) override;
			virtual void Bind() const override;
			virtual void Dispose() override;

			inline VkImage GetImage() const { return m_image; }
			inline VkImageView GetImageView() const { return m_imageView; }
			inline VkSampler GetSampler() const { return m_sampler; }

		private:
			VkImage m_image;
			VkImageView m_imageView;
			VmaAllocation m_allocation;
			VkSampler m_sampler;
		};
	}
}