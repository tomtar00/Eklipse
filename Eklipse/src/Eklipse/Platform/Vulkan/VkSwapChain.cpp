#include "precompiled.h"
#include "Vk.h"

#include "VkSwapChain.h"
#include "VkUtils.h"
#include "VkImage.h"

#include <Eklipse/Core/Application.h>
#include <Eklipse/Renderer/Settings.h>

namespace Eklipse
{
	namespace Vulkan
	{
        VkSwapchainKHR				g_swapChain = VK_NULL_HANDLE;
        VkFormat					g_swapChainImageFormat;
        VkExtent2D					g_swapChainExtent;
        uint32_t                    g_swapChainImageCount;
        std::vector<VkImage>		g_swapChainImages;
        std::vector<VkImageView>	g_swapChainImageViews;
        std::vector<VkFramebuffer>	g_swapChainFramebuffers;

        VkSwapchainKHR CreateSwapChain(int frameWidth, int frameHeight, uint32_t& minImageCount, 
            VkFormat& imageFormat, VkExtent2D& extent, std::vector<VkImage>& images)
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(g_physicalDevice);
            VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
            VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
            extent = ChooseSwapExtent(swapChainSupport.capabilities, frameWidth, frameHeight);

            minImageCount = swapChainSupport.capabilities.minImageCount + 1;
            if (swapChainSupport.capabilities.maxImageCount > 0 && minImageCount > swapChainSupport.capabilities.maxImageCount)
            {
                minImageCount = swapChainSupport.capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = g_surface;
            createInfo.minImageCount = minImageCount;
            createInfo.imageFormat = surfaceFormat.format;
            createInfo.imageColorSpace = surfaceFormat.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            uint32_t queueFamilyIndices[] =
            {
                g_queueFamilyIndices.graphicsAndComputeFamily,
                g_queueFamilyIndices.presentFamily
            };

            if (g_queueFamilyIndices.graphicsAndComputeFamily != g_queueFamilyIndices.presentFamily)
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

            VkSwapchainKHR swapchain;
            VkResult res;
            res = vkCreateSwapchainKHR(g_logicalDevice, &createInfo, nullptr, &swapchain);
            HANDLE_VK_RESULT(res, "CREATE SWAPCHAIN");

            res = vkGetSwapchainImagesKHR(g_logicalDevice, swapchain, &minImageCount, nullptr);
            HANDLE_VK_RESULT(res, "GET SWAPCHAIN IMAGES COUNT");

            images.resize(minImageCount);
            res = vkGetSwapchainImagesKHR(g_logicalDevice, swapchain, &minImageCount, images.data());
            HANDLE_VK_RESULT(res, "GET SWAPCHAIN IMAGES");

            imageFormat = surfaceFormat.format;
            return swapchain;
        }

        void CreateSwapChainImageViews(std::vector<VkImageView>& imageViews, std::vector<VkImage>& images)
        {
            imageViews.resize(images.size());
            for (size_t i = 0; i < images.size(); i++)
            {
                imageViews[i] = ICreateImageView
                (
                    images[i], g_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1
                );
            }
        }

        void CreateFrameBuffers(std::vector<VkFramebuffer>& framebuffers, std::vector<VkImageView>& imageViews, VkRenderPass renderPass, VkExtent2D extent)
        {
            framebuffers.resize(imageViews.size());

            for (size_t i = 0; i < imageViews.size(); i++)
            {
                std::vector<VkImageView> attachments;

                if (RendererSettings::msaaSamples != VK_SAMPLE_COUNT_1_BIT)
                {
                    attachments =
                    {
                        g_colorImage.m_imageView,
                        g_depthImage.m_imageView,
                        imageViews[i]
                    };
                }
                else
                {
                    attachments =
                    {
                        imageViews[i],
                        g_depthImage.m_imageView
                    };
                }

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = renderPass;
                framebufferInfo.attachmentCount = attachments.size();
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = extent.width;
                framebufferInfo.height = extent.height;
                framebufferInfo.layers = 1;

                VkResult res = vkCreateFramebuffer(g_logicalDevice, &framebufferInfo, nullptr, &framebuffers[i]);
                HANDLE_VK_RESULT(res, "CREATE FRAMEBUFFER");
            }
        }
        void DisposeSwapchain()
        {
            for (auto framebuffer : g_swapChainFramebuffers)
            {
                vkDestroyFramebuffer(g_logicalDevice, framebuffer, nullptr);
            }
            for (auto imageView : g_swapChainImageViews)
            {
                vkDestroyImageView(g_logicalDevice, imageView, nullptr);
            }
            vkDestroySwapchainKHR(g_logicalDevice, g_swapChain, nullptr);
        }

        void DestroyFrameBuffers(std::vector<VkFramebuffer>& buffers)
        {
            for (auto framebuffer : buffers)
            {
                vkDestroyFramebuffer(g_logicalDevice, framebuffer, nullptr);
            }
        }
        void DestroyImageViews(std::vector<VkImageView>& imageViews)
        {
            for (auto imageView : imageViews)
            {
                vkDestroyImageView(g_logicalDevice, imageView, nullptr);
            }
        }
	}
}
