#include <precompiled.h>
#include "VK.h"

#include "VKUtils.h"

namespace Eklipse
{
	namespace Vulkan
	{
		bool QueueFamilyIndices::isComplete()
		{
			return has_graphicsAndComputeFamily && has_presentFamily;
		}

		VkImageAspectFlagBits ConvertToVKAspect(ImageAspect aspect)
		{
			switch (aspect)
			{
				case ImageAspect::COLOR:			return VK_IMAGE_ASPECT_COLOR_BIT;
				case ImageAspect::DEPTH:			return VK_IMAGE_ASPECT_DEPTH_BIT;
				case ImageAspect::STENCIL:			return VK_IMAGE_ASPECT_STENCIL_BIT;
			}

			EK_ASSERT(false, "Wrong image aspect");
			return VK_IMAGE_ASPECT_NONE;
		}
		VkImageUsageFlagBits ConvertToVKUsage(uint32_t internalUsage)
		{
			uint32_t usageFlag = 0;
			if (internalUsage & ImageUsage::SAMPLED)			usageFlag |= VK_IMAGE_USAGE_SAMPLED_BIT;
			if (internalUsage & ImageUsage::COLOR_ATTACHMENT)	usageFlag |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			if (internalUsage & ImageUsage::DEPTH_ATTACHMENT)	usageFlag |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			if (internalUsage & ImageUsage::TRANSFER_SRC)		usageFlag |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			if (internalUsage & ImageUsage::TRASNFER_DST)		usageFlag |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

			EK_ASSERT(usageFlag != 0, "Wrong image usage");
			return (VkImageUsageFlagBits)usageFlag;
		}
		VkImageLayout ConvertToVKLayout(Eklipse::ImageLayout layout)
		{
			switch (layout)
			{
				case ImageLayout::COLOR_OPTIMAL:		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				case ImageLayout::DEPTH_OPTIMAL:		return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				case ImageLayout::SHADER_READ_ONLY:		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}

			EK_ASSERT(false, "Wrong image layout");
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
		VkFormat ConvertToVKFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::R8:				return VK_FORMAT_R8_SRGB;
				case ImageFormat::RGB8:				return VK_FORMAT_R8G8B8_SRGB;
				case ImageFormat::RGBA8:			return VK_FORMAT_R8G8B8A8_SRGB;
				case ImageFormat::BGRA8:			return VK_FORMAT_B8G8R8A8_SRGB;
				case ImageFormat::RGBA32F:			return VK_FORMAT_R32G32B32A32_SFLOAT;
				case ImageFormat::D24S8:			return VK_FORMAT_D24_UNORM_S8_UINT;
			}

			EK_ASSERT(false, "Wrong image format");
			return VK_FORMAT_UNDEFINED;
		}

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Vec<VkSurfaceFormatKHR>& availableFormats, VkFormat desiredFormat)
		{
			for (const auto& availableFormat : availableFormats)
			{
				if (availableFormat.format == desiredFormat /*&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR*/)
				{
					return availableFormat;
				}
			}

			return availableFormats[0];
		}
		VkPresentModeKHR ChooseSwapPresentMode(const Vec<VkPresentModeKHR>& availablePresentModes)
		{
			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return availablePresentMode;
				}
			}

			return VK_PRESENT_MODE_FIFO_KHR;
		}
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int frameWidth, int frameHeight)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				VkExtent2D actualExtent =
				{
					static_cast<uint32_t>(frameWidth),
					static_cast<uint32_t>(frameHeight)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}
		VkShaderModule CreateShaderModule(const Vec<uint32_t>& code)
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size() * sizeof(uint32_t);
			createInfo.pCode = code.data();

			VkShaderModule shaderModule;
			VkResult res = vkCreateShaderModule(g_logicalDevice, &createInfo, nullptr, &shaderModule);
			HANDLE_VK_RESULT(res, "CREATE SHADER MODULE");

			return shaderModule;
		}
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device)
		{
			SwapChainSupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, g_surface, &details.capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, g_surface, &formatCount, nullptr);
			if (formatCount != 0)
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, g_surface, &formatCount, details.formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, g_surface, &presentModeCount, nullptr);
			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, g_surface, &presentModeCount, details.presentModes.data());
			}

			return details;
		}
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices{};

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			Vec<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
				{
					indices.graphicsAndComputeFamily = i;
					indices.has_graphicsAndComputeFamily = true;
				}

				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, g_surface, &presentSupport);

				if (presentSupport)
				{
					indices.presentFamily = i;
					indices.has_presentFamily = true;
				}

				i++;
			}

			return indices;
		}
		VkSampleCountFlagBits GetMaxUsableSampleCount()
		{
			VkPhysicalDeviceProperties physicalDeviceProperties;
			vkGetPhysicalDeviceProperties(g_physicalDevice, &physicalDeviceProperties);

			VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
			if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
			if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
			if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
			if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
			if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
			if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

			return VK_SAMPLE_COUNT_1_BIT;
		}
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
		{
			for (uint32_t i = 0; i < g_physicalDeviceMemoryProps.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (g_physicalDeviceMemoryProps.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}

			EK_ASSERT(false, "Failed to find memory type!");
		}
		VkFormat FindSupportedFormat(const Vec<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
		{
			for (VkFormat format : candidates)
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(g_physicalDevice, format, &props);

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
		VkFormat FindDepthFormat()
		{
			return FindSupportedFormat(
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			);
		}
		bool HasStencilComponent(VkFormat format)
		{
			return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
		}
	}
}