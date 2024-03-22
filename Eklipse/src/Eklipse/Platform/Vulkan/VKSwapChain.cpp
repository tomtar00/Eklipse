#include "precompiled.h"
#include "Vk.h"

#include "VKSwapChain.h"
#include "VKUtils.h"

#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	namespace Vulkan
	{
        VkSwapchainKHR				g_swapChain = VK_NULL_HANDLE;
        VkFormat					g_swapChainImageFormat;
        VkPresentModeKHR            g_swapChainPresentMode;
        VkExtent2D					g_swapChainExtent;
        uint32_t                    g_swapChainImageCount;
        Vec<VkImage>		g_swapChainImages;
        Vec<VkImageView>	g_swapChainImageViews;

        VkSwapchainKHR CreateSwapChain(int frameWidth, int frameHeight, uint32_t& minImageCount, 
            VkFormat& imageFormat, VkPresentModeKHR& presentMode, VkExtent2D& extent, Vec<VkImage>& images)
        {
            EK_CORE_PROFILE();
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(g_physicalDevice);
            VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats, imageFormat);
            VkPresentModeKHR surfacePresentMode = ChooseSwapPresentMode(swapChainSupport.presentModes, presentMode);
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
            createInfo.presentMode = surfacePresentMode;
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

        void CreateImages(Vec<VkImage>& images, Vec<VmaAllocation>& allocations, int numImages, uint32_t width, uint32_t height, uint32_t mipLevels,
            VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags usage)
        {
            EK_CORE_PROFILE();
            allocations.resize(numImages);
            images.resize(numImages);
            for (int i = 0; i < numImages; i++)
            {
                images[i] = CreateImage(allocations[i], width, height, mipLevels, numSamples,
                    format, tiling, usage);
            }
        }
        void CreateImageViews(Vec<VkImageView>& imageViews, Vec<VkImage>& images, VkFormat format)
        {
            EK_CORE_PROFILE();
            imageViews.resize(images.size());
            for (size_t i = 0; i < images.size(); i++)
            {
                imageViews[i] = CreateImageView( images[i], format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
            }
        }
        void CreateSamplers(Vec<VkSampler>& samplers, int numSamplers)
        {
            EK_CORE_PROFILE();
            samplers.resize(numSamplers);
            for (int i = 0; i < numSamplers; i++)
            {
                samplers[i] = CreateSampler(0.0f);
            }
        }
        void CreateFrameBuffers(Vec<VkFramebuffer>& framebuffers, Vec<VkImageView>& imageViews, VkRenderPass renderPass, VkExtent2D extent, bool attachOnlyView = false)
        {
            EK_CORE_PROFILE();
            framebuffers.resize(imageViews.size());
            for (size_t i = 0; i < imageViews.size(); i++)
            {
                Vec<VkImageView> attachments;

                if (attachOnlyView)
                {
                    attachments =
                    {
                        imageViews[i]
                    };
                }
                else
                {
                    if (Renderer::GetSettings().GetMsaaSamples() > 1)
                    {
                        attachments =
                        {
                            //g_colorImage.m_imageView,
                            //g_depthImage.m_imageView,
                            imageViews[i]
                        };
                    }
                    else
                    {
                        attachments =
                        {
                            imageViews[i],
                            //g_depthImage.m_imageView
                        };
                    }
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

        void DestroyImages(Vec<VkImage>& images, Vec<VmaAllocation>& allocations)
        {
            EK_CORE_PROFILE();
            int i = 0;
            for (auto image : images)
            {
                vmaDestroyImage(g_allocator, image, allocations[i++]);
            }
        }
        void DestroyImageViews(Vec<VkImageView>& imageViews)
        {
            EK_CORE_PROFILE();
            for (auto imageView : imageViews)
            {
                vkDestroyImageView(g_logicalDevice, imageView, nullptr);
            }
        }
        void DestroySamplers(Vec<VkSampler>& samplers)
        {
            EK_CORE_PROFILE();
            for (auto sampler : samplers)
            {
                vkDestroySampler(g_logicalDevice, sampler, nullptr);
            }
        }
        void DestroyFrameBuffers(Vec<VkFramebuffer>& buffers)
        {
            EK_CORE_PROFILE();
            for (auto framebuffer : buffers)
            {
                vkDestroyFramebuffer(g_logicalDevice, framebuffer, nullptr);
            }
        }
    }
}
