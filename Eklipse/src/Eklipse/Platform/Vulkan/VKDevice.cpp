#include "precompiled.h"
#include "VK.h"
#include "VKDevice.h"
#include "VKUtils.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VkDevice                    g_logicalDevice    = VK_NULL_HANDLE;
		VkPhysicalDevice            g_physicalDevice   = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties  g_physicalDeviceProps{};
        VkPhysicalDeviceMemoryProperties g_physicalDeviceMemoryProps{};

        const Vec<const char*>   g_deviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

		void CreateLogicalDevice()
		{      
            EK_CORE_PROFILE();
            Vec<VkDeviceQueueCreateInfo> queueCreateInfos;
            std::set<uint32_t> uniqueQueueFamilies = 
            {
                g_queueFamilyIndices.graphicsAndComputeFamily,
                g_queueFamilyIndices.presentFamily
            };

            float queuePriority = 1.0f;
            for (uint32_t queueFamily : uniqueQueueFamilies)
            {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;

                queueCreateInfos.push_back(queueCreateInfo);
            }

            VkPhysicalDeviceFeatures deviceFeatures{};
            deviceFeatures.samplerAnisotropy = VK_TRUE;
            deviceFeatures.sampleRateShading = VK_TRUE;
            deviceFeatures.fillModeNonSolid = VK_TRUE;
            deviceFeatures.wideLines = VK_TRUE;

            VkDeviceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.queueCreateInfoCount = queueCreateInfos.size();
            createInfo.pQueueCreateInfos = queueCreateInfos.data();
            createInfo.pEnabledFeatures = &deviceFeatures;

            createInfo.enabledExtensionCount = g_deviceExtensions.size();
            createInfo.ppEnabledExtensionNames = g_deviceExtensions.data();

            if (g_validationLayersEnabled)
            {
                createInfo.enabledLayerCount = g_validationLayers.size();
                createInfo.ppEnabledLayerNames = g_validationLayers.data();
            }
            else
            {
                createInfo.enabledLayerCount = 0;
            }

            VkResult res = vkCreateDevice(g_physicalDevice, &createInfo, nullptr, &g_logicalDevice);
            HANDLE_VK_RESULT(res, "CREATE LOGICAL DEVICE");

            vkGetDeviceQueue(g_logicalDevice, g_queueFamilyIndices.graphicsAndComputeFamily, 0, &g_graphicsQueue);
            vkGetDeviceQueue(g_logicalDevice, g_queueFamilyIndices.graphicsAndComputeFamily, 0, &g_computeQueue);
            vkGetDeviceQueue(g_logicalDevice, g_queueFamilyIndices.presentFamily, 0, &g_presentQueue);
		}
		void PickPhysicalDevice()
		{
            EK_CORE_PROFILE();
			VkResult res;

			uint32_t gpuCount;
			res = vkEnumeratePhysicalDevices(g_instance, &gpuCount, nullptr);
			HANDLE_VK_RESULT(res, "GET NUMBER OF PHYSICAL DEVICES");

			Vec<VkPhysicalDevice> gpus(gpuCount);
			res = vkEnumeratePhysicalDevices(g_instance, &gpuCount, gpus.data());
			HANDLE_VK_RESULT(res, "ALLOCATE PHYSICAL DEVICES VECTOR");

			for (uint32_t i = 0; i < gpuCount; i++)
			{
				vkGetPhysicalDeviceProperties(gpus[i], &g_physicalDeviceProps);

				if (g_physicalDeviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					g_physicalDevice = gpus[i];
					break;
				}
			}
		}
	}
}
