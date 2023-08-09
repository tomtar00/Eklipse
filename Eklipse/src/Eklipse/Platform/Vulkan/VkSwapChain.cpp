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

		void SetupSwapchain()
		{
			// Create swapchain
			{
                int width, height;
                Eklipse::Application::Get().GetWindow()->GetFramebufferSize(width, height);

                SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(g_physicalDevice);
                VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
                VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
                VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, width, height);

                g_swapChainImageCount = swapChainSupport.capabilities.minImageCount + 1;

                if (swapChainSupport.capabilities.maxImageCount > 0 && g_swapChainImageCount > swapChainSupport.capabilities.maxImageCount)
                {
                    g_swapChainImageCount = swapChainSupport.capabilities.maxImageCount;
                }

                VkSwapchainCreateInfoKHR createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                createInfo.surface = g_surface;
                createInfo.minImageCount = g_swapChainImageCount;
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

                VkResult res;
                res = vkCreateSwapchainKHR(g_logicalDevice, &createInfo, nullptr, &g_swapChain);
                HANDLE_VK_RESULT(res, "CREATE SWAPCHAIN");

                res = vkGetSwapchainImagesKHR(g_logicalDevice, g_swapChain, &g_swapChainImageCount, nullptr);
                HANDLE_VK_RESULT(res, "GET SWAPCHAIN IMAGES COUNT");
                g_swapChainImages.resize(g_swapChainImageCount);
                res = vkGetSwapchainImagesKHR(g_logicalDevice, g_swapChain, &g_swapChainImageCount, g_swapChainImages.data());
                HANDLE_VK_RESULT(res, "GET SWAPCHAIN IMAGES");

                g_swapChainImageFormat = surfaceFormat.format;
                g_swapChainExtent = extent;
			}

            // Create image views
            {
                g_swapChainImageViews.resize(g_swapChainImages.size());

                for (size_t i = 0; i < g_swapChainImages.size(); i++)
                {
                    g_swapChainImageViews[i] = ICreateImageView
                    (
                        g_swapChainImages[i], g_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1
                    );
                }
            }
		}
        void SetupFramebuffers()
        {
            g_swapChainFramebuffers.resize(g_swapChainImageViews.size());

            for (size_t i = 0; i < g_swapChainImageViews.size(); i++)
            {
                std::vector<VkImageView> attachments;

                if (RendererSettings::msaaSamples != VK_SAMPLE_COUNT_1_BIT)
                {
                    attachments =
                    {
                        g_colorImage.m_imageView,
                        g_depthImage.m_imageView,
                        g_swapChainImageViews[i]
                    };
                }
                else
                {
                    attachments =
                    {
                        g_swapChainImageViews[i],
                        g_depthImage.m_imageView
                    };
                }

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = g_renderPass;
                framebufferInfo.attachmentCount = attachments.size();
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = g_swapChainExtent.width;
                framebufferInfo.height = g_swapChainExtent.height;
                framebufferInfo.layers = 1;

                VkResult res = vkCreateFramebuffer(g_logicalDevice, &framebufferInfo, nullptr, &g_swapChainFramebuffers[i]);
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
	}
}
