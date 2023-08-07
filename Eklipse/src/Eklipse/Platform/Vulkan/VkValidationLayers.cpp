#include "precompiled.h"
#include "VkValidationLayers.h"
#include "_globals.h"
#include "VkUtils.h"

namespace Eklipse
{
	namespace Vulkan
	{
#ifdef EK_DEBUG
		const bool g_validationLayersEnabled = true;
#else
		const bool g_validationLayersEnabled = false;
#endif

		VkDebugUtilsMessengerEXT g_debugMessanger			= VK_NULL_HANDLE;
		const std::vector<const char*> g_validationLayers	= { "VK_LAYER_KHRONOS_validation"};

		void SetupValidationLayers()
		{
			if (!g_validationLayersEnabled) return;

			VkDebugUtilsMessengerCreateInfoEXT createInfo;
			createInfo.flags = 0;
			createInfo.pNext = NULL;
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = DebugCallback;

			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr)
			{
				func(g_instance, &createInfo, nullptr, &g_debugMessanger);
			}
			else
			{
				HANDLE_VK_RESULT(VK_ERROR_EXTENSION_NOT_PRESENT, "SETUP VALIDATION LAYERS");
			}
		}
		VkResult CheckValidationLayersSupport()
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			for (const char* layerName : g_validationLayers)
			{
				bool layerFound = false;

				for (const auto& layerProperties : availableLayers)
				{
					if (strcmp(layerName, layerProperties.layerName) == 0)
					{
						layerFound = true;
						break;
					}
				}

				if (!layerFound)
				{
					return VK_ERROR_LAYER_NOT_PRESENT;
				}
			}

			return VK_SUCCESS;
		}
		void PopulateInstanceCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo, VkInstanceCreateInfo* createInfo)
		{
			createInfo->enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size());
			createInfo->ppEnabledLayerNames = g_validationLayers.data();
			debugCreateInfo.flags = 0;
			debugCreateInfo.pNext = NULL;
			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCreateInfo.pfnUserCallback = DebugCallback;
			createInfo->pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		{
			switch (messageSeverity)
			{
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				{
					EK_CORE_TRACE("Valition layer: {0}", pCallbackData->pMessage);
					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				{
					EK_CORE_INFO("Valition layer: {0}", pCallbackData->pMessage);
					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				{
					EK_CORE_WARN("Valition layer: {0}", pCallbackData->pMessage);
					break;
				}
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				{
					EK_CORE_ERROR("Valition layer: {0}", pCallbackData->pMessage);
					break;
				}
			}
			return VK_FALSE;
		}
		void DisposeValidationLayers()
		{
			if (!g_validationLayersEnabled) return;

			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
			{
				func(g_instance, g_debugMessanger, nullptr);
			}
		}
	}
}
