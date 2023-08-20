#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	namespace Vulkan
	{
		void SetupValidationLayers();
		VkResult CheckValidationLayersSupport();
		void PopulateInstanceCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo, VkInstanceCreateInfo* createInfo);
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);
		void DestroyValidationLayers();
	}
}