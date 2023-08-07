#pragma once
#include <vulkan/vulkan.h>

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
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		void HANDLE_VK_RESULT(VkResult result, const char* name);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int frameWidth, int frameHeight);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		VkSampleCountFlagBits GetMaxUsableSampleCount();
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat();
		bool HasStencilComponent(VkFormat format);
	}
}