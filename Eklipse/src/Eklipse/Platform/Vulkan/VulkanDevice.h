#pragma once
#include <vulkan/vulkan.h>

namespace Eklipse
{
	class VulkanDevice
	{
	public:
		VulkanDevice();

		void Init();
		void Shutdown();

		VkDevice& Device() { return m_device; }
		VkPhysicalDevice& PhysicalDevice() { return m_physicalDevice; }

	private:
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		int RateDeviceSuitability(VkPhysicalDevice device);

		VkDevice m_device{};
		VkPhysicalDevice m_physicalDevice{};

		const std::vector<const char*> m_deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};
}