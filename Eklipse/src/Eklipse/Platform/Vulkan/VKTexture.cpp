#include "precompiled.h"
#include "VK.h"
#include "VKTexture.h"
#include "VKUtils.h"
#include <stb_image.h>
#include "VKCommands.h"
#include "VKBuffers.h"

namespace Eklipse
{
    namespace Vulkan
    {
        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
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

            VkImageView imageView;
            VkResult res = vkCreateImageView(g_logicalDevice, &viewInfo, nullptr, &imageView);
            HANDLE_VK_RESULT(res, "CREATE IMAGE VIEW");

            return imageView;
        }
        VkSampler CreateSampler(float mipLevels)
        {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;

            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = g_physicalDeviceProps.limits.maxSamplerAnisotropy;

            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.minLod = 0.0f; // Optional
            samplerInfo.maxLod = mipLevels;
            samplerInfo.mipLodBias = 0.0f; // Optional

            VkSampler sampler;
            VkResult res = vkCreateSampler(g_logicalDevice, &samplerInfo, nullptr, &sampler);
            HANDLE_VK_RESULT(res, "CREATE SAMPLER");

            return sampler;
        }
        VkImage CreateImage(VmaAllocation& allocation, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
        {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.samples = numSamples;
            imageInfo.mipLevels = mipLevels;

            VmaAllocationCreateInfo vmaAllocInfo = {};
            vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
            vmaAllocInfo.priority = 1.0f;

            VkImage image;
            VkResult res = vmaCreateImage(g_allocator, &imageInfo, &vmaAllocInfo, &image, &allocation, nullptr);
            HANDLE_VK_RESULT(res, "CREATE IMAGE");

            return image;
        }
        
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
        {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
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
            else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else
            {
                EK_ASSERT(false, "Unsuported layout transition from {0} to {1}", int(oldLayout), int(newLayout));
            }

            VkCommandBuffer commandBuffer = BeginSingleCommands();

            vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );

            EndSingleCommands(commandBuffer);
        }
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
        {
            VkCommandBuffer commandBuffer = BeginSingleCommands();

            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { width, height, 1 };

            vkCmdCopyBufferToImage(
                commandBuffer,
                buffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region
            );

            EndSingleCommands(commandBuffer);
        }
        void GenerateMipMaps(VkImage image, uint32_t mipLevels, uint32_t width, uint32_t height)
        {
            if (mipLevels < 2) return;

            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(g_physicalDevice, VK_FORMAT_R8G8B8A8_SRGB, &formatProperties);
            EK_ASSERT((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT), "texture image format does not support linear blitting!");

            VkCommandBuffer commandBuffer = BeginSingleCommands();

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

            EndSingleCommands(commandBuffer);
        }

        VKTexture2D::VKTexture2D(const Path& path, const AssetHandle handle) : Texture2D(path, handle)
        {
            TextureData textureData{};
            if (LoadTextureFromFile(path, textureData))
            {
                m_textureInfo = textureData.info;
                Init(m_textureInfo);
                SetData(textureData.data, textureData.size);
            }
        }
        VKTexture2D::VKTexture2D(const TextureInfo& textureInfo) : Texture2D(textureInfo)
        {
            m_textureInfo = textureInfo;
            Init(m_textureInfo);
        }
        VKTexture2D::VKTexture2D(const TextureData& textureData) : Texture2D(textureData)
        {
            m_textureInfo = textureData.info;
            Init(m_textureInfo);
            SetData(textureData.data, textureData.size);
        }

        void VKTexture2D::Init(const TextureInfo& textureInfo)
        {
            VkFormat format = ConvertToVKFormat(textureInfo.imageFormat);
            EK_ASSERT(format != VK_FORMAT_UNDEFINED, "Texture format not supported!");

            VkImageAspectFlagBits aspect = ConvertToVKAspect(m_textureInfo.imageAspect);
            VkImageUsageFlagBits usage = ConvertToVKUsage(m_textureInfo.imageUsage);

            m_image = CreateImage(m_allocation, textureInfo.width, textureInfo.height, textureInfo.mipMapLevel,
                VK_SAMPLE_COUNT_1_BIT, format, VK_IMAGE_TILING_OPTIMAL, usage);

            if (textureInfo.imageLayout != ImageLayout::LAYOUT_UNDEFINED)
            {
                VkImageLayout layout = ConvertToVKLayout(textureInfo.imageLayout);
                TransitionImageLayout(m_image, format, VK_IMAGE_LAYOUT_UNDEFINED, layout, textureInfo.mipMapLevel);
            }

            m_imageView = CreateImageView(m_image, format, aspect, textureInfo.mipMapLevel);
            m_sampler = CreateSampler(textureInfo.mipMapLevel);
        }
        void VKTexture2D::SetData(void* data, uint32_t size)
        {
            uint32_t singlePixelSize = FormatToChannels(m_textureInfo.imageFormat);
            uint32_t dataSize = m_textureInfo.width * m_textureInfo.height * singlePixelSize;
            EK_ASSERT(size == dataSize, "Data is not equal required size of the texture! Given: {0} Required: {1}", size, dataSize);

            {
                VKStagingBuffer stagingBuffer(data, size);
                VkFormat format = ConvertToVKFormat(m_textureInfo.imageFormat);
                EK_ASSERT(format != VK_FORMAT_UNDEFINED, "Texture format not supported!");

                TransitionImageLayout(m_image, format, VK_IMAGE_LAYOUT_UNDEFINED,
                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_textureInfo.mipMapLevel);

                CopyBufferToImage(stagingBuffer.m_buffer, m_image, m_textureInfo.width, m_textureInfo.height);

                // if mip maps are disabled - uncomment
                TransitionImageLayout(m_image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_textureInfo.mipMapLevel);
            }

            //GenerateMipMaps(m_image, m_textureInfo.mipMapLevel, m_textureInfo.width, m_textureInfo.height);
        }
        void VKTexture2D::Bind() const {}
        void VKTexture2D::Unbind() const {}
        void VKTexture2D::Dispose()
        {
            vkDestroySampler(g_logicalDevice, m_sampler, nullptr);
            vkDestroyImageView(g_logicalDevice, m_imageView, nullptr);
            vmaDestroyImage(g_allocator, m_image, m_allocation);
        }
    }
}