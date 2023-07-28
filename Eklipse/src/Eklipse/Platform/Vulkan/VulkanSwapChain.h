#pragma once
#include "VulkanAPI.h"

#include <vulkan/vulkan.h>

namespace Eklipse
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapChain
	{
	public:
		void InitChainViews();
		void InitFramebuffers();
		void Shutdown();

	public:
		VkSwapchainKHR& SwapChain();
		VkFormat& Format();
		VkExtent2D& Extent();
		std::vector<VkFramebuffer>& Framebuffers();

	private:
		void CreateSwapChain();
		void CreateImageViews();
		void CreateFramebuffers();

	private:
		VkSwapchainKHR m_swapChain{};
		VkFormat m_swapChainImageFormat{};
		VkExtent2D m_swapChainExtent{};
		std::vector<VkImage> m_swapChainImages{};
		std::vector<VkImageView> m_swapChainImageViews{};
		std::vector<VkFramebuffer> m_swapChainFramebuffers{};
	};
}