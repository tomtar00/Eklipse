#include "precompiled.h"

#include "VulkanDevice.h"
#include "VulkanAPI.h"

#include <vulkan/vulkan.h>

namespace Eklipse
{
    VulkanDevice::VulkanDevice() {}

    void VulkanDevice::Init()
    {
        PickPhysicalDevice();
        CreateLogicalDevice();
    }
    void VulkanDevice::Shutdown()
    {
        vkDestroyDevice(m_device, nullptr);
    }

    void VulkanDevice::PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(VulkanAPI::Get().Instance(), &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(VulkanAPI::Get().Instance(), &deviceCount, devices.data());

        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto& device : devices)
        {
            int score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0)
        {
            m_physicalDevice = candidates.rbegin()->second;
        }
        else
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }
    void VulkanDevice::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = VulkanAPI::Get().FindQueueFamilies(
            VulkanAPI::Get().Devices().PhysicalDevice()
        );

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily,
            indices.presentFamily
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

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();     

        if (g_validationLayersEnabled)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanAPI::Get().ValidationLayers().Layers().size());
            createInfo.ppEnabledLayerNames = VulkanAPI::Get().ValidationLayers().Layers().data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_device, indices.graphicsFamily, 0, &VulkanAPI::Get().GraphicsQueue());
        vkGetDeviceQueue(m_device, indices.presentFamily, 0, &VulkanAPI::Get().PresentQueue());
    }
    bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }
    int VulkanDevice::RateDeviceSuitability(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        int score = 0;

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }

        score += deviceProperties.limits.maxImageDimension2D;
        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = VulkanAPI::Get().QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        if (!deviceFeatures.geometryShader ||
            !VulkanAPI::Get().FindQueueFamilies(device).isComplete() ||
            !extensionsSupported ||
            !swapChainAdequate ||
            !supportedFeatures.samplerAnisotropy)
        {
            return 0;
        }

        return score;
    }
}
