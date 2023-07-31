#include "precompiled.h"

#include "VulkanTexture.h"
#include "VulkanBuffers.h"
#include "VulkanAPI.h"
#include <vulkan/vulkan.h>
#include <stb_image.h>

namespace Eklipse
{
	/////////////////////////////////////////////////
	// IMAGE ////////////////////////////////////////
	/////////////////////////////////////////////////

	void VulkanImage::Init(uint32_t width, uint32_t height, uint32_t mipLevels,
		VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(width);
		imageInfo.extent.height = static_cast<uint32_t>(height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = numSamples;
		imageInfo.mipLevels = mipLevels;

		VkDevice device = VulkanAPI::Get().Devices().Device();
		if (vkCreateImage(device, &imageInfo, nullptr, &m_image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, m_image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanBuffer::FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, m_image, m_imageMemory, 0);
	}
	void VulkanImage::Shutdown()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkDestroyImageView(device, m_imageView, nullptr);
		vkDestroyImage(device, m_image, nullptr);
		vkFreeMemory(device, m_imageMemory, nullptr);
	}
	VkImage& VulkanImage::Image()
	{
		return m_image;
	}
	VkDeviceMemory& VulkanImage::Memory()
	{
		return m_imageMemory;
	}
	VkImageView& VulkanImage::ImageView()
	{
		return m_imageView;
	}
	void VulkanImage::TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
	{
		VkCommandBuffer commandBuffer = VulkanAPI::Get().CommandPool().BeginSingleCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = mipLevels;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		VulkanAPI::Get().CommandPool().EndSingleCommands(commandBuffer);
	}
	void VulkanImage::CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = VulkanAPI::Get().CommandPool().BeginSingleCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			m_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		VulkanAPI::Get().CommandPool().EndSingleCommands(commandBuffer);
	}
	void VulkanImage::CreateImage(VkDeviceSize imageSize, uint32_t width, uint32_t height, uint32_t mipLevels, const void* data)
	{
		VulkanStagingBuffer stagingBuffer;
		stagingBuffer.Init(imageSize, data);

		Init(width, height, mipLevels,
			VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

		CopyBufferToImage(stagingBuffer.Buffer(), width, height);

		// if mip maps are disabled, uncomment
		//TransitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		//   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

		stagingBuffer.Shutdown();
	}
	void VulkanImage::CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
	{
		m_imageView = CreateImageView(m_image, format, aspectFlags, mipLevels);
	}
	VkImageView VulkanImage::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.levelCount = mipLevels;

		VkDevice device = VulkanAPI::Get().Devices().Device();
		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	/////////////////////////////////////////////////
	// TEXTURE //////////////////////////////////////
	/////////////////////////////////////////////////

	void VulkanTexture::Load(char* path)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		EK_ASSERT(pixels, "Failed to load texture image from location: {0}", path);

		m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

		m_image.CreateImage(imageSize, texWidth, texHeight, m_mipLevels, pixels);
		GenerateMipMaps(m_mipLevels, texWidth, texHeight);

		stbi_image_free(pixels);

		m_image.CreateImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
		CreateSampler();
	}
	void VulkanTexture::Shutdown()
	{
		VkDevice device = VulkanAPI::Get().Devices().Device();
		vkDestroySampler(device, m_textureSampler, nullptr);
		m_image.Shutdown();
	}
	VulkanImage& VulkanTexture::Image()
	{
		return m_image;
	}
	VkSampler& VulkanTexture::Sampler()
	{
		return m_textureSampler;
	}
	void VulkanTexture::CreateSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkPhysicalDevice physicalDevice = VulkanAPI::Get().Devices().PhysicalDevice();
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f; // Optional
		samplerInfo.maxLod = static_cast<float>(m_mipLevels);
		samplerInfo.mipLodBias = 0.0f; // Optional

		VkDevice device = VulkanAPI::Get().Devices().Device();
		if (vkCreateSampler(device, &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler!");
		}
	}
	void VulkanTexture::GenerateMipMaps(uint32_t mipLevels, uint32_t width, uint32_t height)
	{
		VkPhysicalDevice physicalDevice = VulkanAPI::Get().Devices().PhysicalDevice();
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, VK_FORMAT_R8G8B8A8_SRGB, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		VkCommandBuffer commandBuffer = VulkanAPI::Get().CommandPool().BeginSingleCommands();

		VkImage& image = m_image.Image();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = width;
		int32_t mipHeight = height;

		for (uint32_t i = 1; i < mipLevels; i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR
			);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		VulkanAPI::Get().CommandPool().EndSingleCommands(commandBuffer);
	}

	/////////////////////////////////////////////////
	// DEPTH IMAGE //////////////////////////////////
	/////////////////////////////////////////////////

	void VulkanDepthImage::Init(VkSampleCountFlagBits numSamples)
	{
		CreateDepthImage(numSamples);
	}
	void VulkanDepthImage::CreateDepthImage(VkSampleCountFlagBits numSamples)
	{
		VkFormat depthFormat = FindDepthFormat();

		VkExtent2D extent = VulkanAPI::Get().SwapChain().Extent();
		VulkanImage::Init(extent.width, extent.height, 1, 
			numSamples, depthFormat,
			VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		CreateImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}
	VkFormat VulkanDepthImage::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		VkPhysicalDevice physicalDevice = VulkanAPI::Get().Devices().PhysicalDevice();
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
	}
	VkFormat VulkanDepthImage::FindDepthFormat()
	{
		return FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}
	bool VulkanDepthImage::HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
	
	/////////////////////////////////////////////////
	// COLOR IMAGE //////////////////////////////////
	/////////////////////////////////////////////////
	
	void VulkanColorImage::Init(VkSampleCountFlagBits numSamples)
	{
		CreateColorImage(numSamples);
	}

	void VulkanColorImage::CreateColorImage(VkSampleCountFlagBits numSamples)
	{
		VkFormat colorFormat = VulkanAPI::Get().SwapChain().Format();

		VkExtent2D extent = VulkanAPI::Get().SwapChain().Extent();
		VulkanImage::Init(extent.width, extent.height, 1,
			numSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		CreateImageView(colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}
