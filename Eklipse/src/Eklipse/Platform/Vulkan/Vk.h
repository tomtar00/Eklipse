#pragma once
#include <vulkan/vulkan.h>
#include "VkImage.h"
#include "VkUtils.h"

#include <vk_mem_alloc.h>

namespace Eklipse
{
	namespace Vulkan
	{
		extern const uint32_t g_maxFramesInFlight;

		extern VkInstance g_instance;
		extern VkSurfaceKHR g_surface;
		extern VmaAllocator g_allocator;

		extern const bool g_validationLayersEnabled;
		extern VkDebugUtilsMessengerEXT g_debugMessanger;
		extern const std::vector<const char*> g_validationLayers;

		extern VkDevice g_logicalDevice;
		extern const std::vector<const char*> g_deviceExtensions;
		extern VkPhysicalDevice g_physicalDevice;
		extern VkPhysicalDeviceProperties g_physicalDeviceProps;
		extern VkPhysicalDeviceMemoryProperties g_physicalDeviceMemoryProps;

		extern QueueFamilyIndices g_queueFamilyIndices;
		extern VkQueue g_graphicsQueue;
		extern VkQueue g_computeQueue;
		extern VkQueue g_presentQueue;

		extern ColorImage g_colorImage;
		extern DepthImage g_depthImage;

		extern VkSwapchainKHR g_swapChain;
		extern VkFormat g_swapChainImageFormat;
		extern VkExtent2D g_swapChainExtent;
		extern uint32_t g_swapChainImageCount;
		extern std::vector<VkImage> g_swapChainImages;
		extern std::vector<VkImageView> g_swapChainImageViews;
		extern std::vector<VkFramebuffer> g_swapChainFramebuffers;

		extern VkDescriptorSetLayout g_graphicsDescriptorSetLayout;
		extern VkDescriptorSetLayout g_computeDescriptorSetLayout;
		extern VkDescriptorPool g_descriptorPool;
		extern std::vector<VkDescriptorSet> g_graphicsDescriptorSets;
		extern std::vector<VkDescriptorSet> g_computeDescriptorSets;

		extern VkCommandPool g_commandPool;
		extern std::vector<VkCommandBuffer> g_drawCommandBuffers;
		extern std::vector<VkCommandBuffer> g_computeCommandBuffers;

		extern VkRenderPass g_renderPass;
		extern VkPipeline g_graphicsPipeline;
		extern VkPipelineLayout g_graphicsPipelineLayout;
		//extern VkPipeline g_particlePipeline;
		//extern VkPipelineLayout g_particlePipelineLayout;
		//extern VkPipeline g_computePipeline;
		//extern VkPipelineLayout g_computePipelineLayout;
	}
}