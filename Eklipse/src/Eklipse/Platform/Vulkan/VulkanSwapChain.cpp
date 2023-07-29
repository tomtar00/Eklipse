#include "precompiled.h"
#include "VulkanAPI.h"
#include "VulkanSwapChain.h"

namespace Eklipse
{
    void VulkanSwapChain::InitChainViews()
    {
        CreateSwapChain();
        CreateImageViews();
    }
    void VulkanSwapChain::InitFramebuffers()
    {
        CreateFramebuffers();
    }
    void VulkanSwapChain::Shutdown()
    {
        VkDevice device = VulkanAPI::Get().Devices().Device();
        for (auto framebuffer : m_swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        for (auto imageView : m_swapChainImageViews)
        {
            vkDestroyImageView(device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(device, m_swapChain, nullptr);
    }

    VkSwapchainKHR& VulkanSwapChain::SwapChain() { return m_swapChain; }
    VkFormat& VulkanSwapChain::Format() { return m_swapChainImageFormat; }
    VkExtent2D& VulkanSwapChain::Extent() { return m_swapChainExtent; }
    std::vector<VkFramebuffer>& VulkanSwapChain::Framebuffers() { return m_swapChainFramebuffers; }

    void VulkanSwapChain::CreateSwapChain()
    {
        VkDevice device = VulkanAPI::Get().Devices().Device();
        VkPhysicalDevice physicalDevice = VulkanAPI::Get().Devices().PhysicalDevice();
        SwapChainSupportDetails swapChainSupport = VulkanAPI::Get().QuerySwapChainSupport(physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = VulkanAPI::Get().ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = VulkanAPI::Get().ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = VulkanAPI::Get().ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = VulkanAPI::Get().Surface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = VulkanAPI::Get().FindQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, nullptr);
        m_swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, m_swapChainImages.data());

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;
    }
    void VulkanSwapChain::CreateImageViews()
    {
        m_swapChainImageViews.resize(m_swapChainImages.size());

        for (size_t i = 0; i < m_swapChainImages.size(); i++)
        {
            m_swapChainImageViews[i] = VulkanImage::CreateImageView(
                m_swapChainImages[i], m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT
            );
        }
    }
    void VulkanSwapChain::CreateFramebuffers()
    {
        m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

        for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
        {
            std::array<VkImageView, 2> attachments = {
                m_swapChainImageViews[i],
                VulkanAPI::Get().DepthImage().ImageView()
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = VulkanAPI::Get().Pipeline().RenderPass();
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_swapChainExtent.width;
            framebufferInfo.height = m_swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(VulkanAPI::Get().Devices().Device(), 
                &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffers!");
            }
        }
    }
}
