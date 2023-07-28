#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	extern const bool g_validationLayersEnabled;

	class VulkanValidationLayers
	{
	public:
		VulkanValidationLayers();

		void Init();
		void Shutdown();
		bool CheckSupport();
		void PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo, VkInstanceCreateInfo* createInfo);

		const std::vector<const char*>& Layers() const;

	private:
		VkResult createMesssenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void destroyMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		VkDebugUtilsMessengerEXT m_debugMessanger{};
	};
}