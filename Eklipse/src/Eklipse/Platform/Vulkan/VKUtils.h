#pragma once
#include <vulkan/vulkan.h>
#include <Eklipse/Renderer/Texture.h>
#include <Eklipse/Renderer/Framebuffer.h>

#define HANDLE_VK_RESULT(res, name) THROW(res == VK_SUCCESS, "Vulkan result not successfull at {0}. Result code = {1}", name, (int)res);

namespace Eklipse
{
	namespace Vulkan
	{
		struct QueueFamilyIndices
		{
			uint32_t graphicsAndComputeFamily;
			bool has_graphicsAndComputeFamily = false;

			uint32_t presentFamily;
			bool has_presentFamily = false;

			bool isComplete();
		};
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			Vec<VkSurfaceFormatKHR> formats;
			Vec<VkPresentModeKHR> presentModes;
		};

		ImageFormat ConvertFromVKFormat(VkFormat format);
		VkFormat ConvertToVKFormat(ImageFormat format);
		VkImageAspectFlagBits ConvertToVKAspect(ImageAspect aspect);
		VkImageUsageFlagBits ConvertToVKUsage(uint32_t internalUsage);
		VkImageLayout ConvertToVKLayout(ImageLayout layout);
		VkPresentModeKHR ConvertToVKPresentMode(PresentMode mode);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Vec<VkSurfaceFormatKHR>& availableFormats, VkFormat desiredFormat);
		VkPresentModeKHR ChooseSwapPresentMode(const Vec<VkPresentModeKHR>& availablePresentModes, VkPresentModeKHR desiredPresentMode);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int frameWidth, int frameHeight);
		VkShaderModule CreateShaderModule(const Vec<uint32_t>& code);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		VkSampleCountFlagBits GetMaxUsableSampleCount();
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkFormat FindSupportedFormat(const Vec<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat();
		bool HasStencilComponent(VkFormat format);
	}
}