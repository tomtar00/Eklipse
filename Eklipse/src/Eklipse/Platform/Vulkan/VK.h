#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "VKUtils.h"
#include "VKFramebuffer.h"

namespace Eklipse
{
	namespace Vulkan
	{
		extern const uint32_t g_maxFramesInFlight;

		extern VkInstance g_instance;
		extern VkSurfaceKHR g_surface;
		extern VmaAllocator g_allocator;

		extern bool g_validationLayersEnabled;
		extern VkDebugUtilsMessengerEXT g_debugMessanger;
		extern const Vec<const char*> g_validationLayers;

		extern VkDevice g_logicalDevice;
		extern const Vec<const char*> g_deviceExtensions;
		extern VkPhysicalDevice g_physicalDevice;
		extern VkPhysicalDeviceProperties g_physicalDeviceProps;
		extern VkPhysicalDeviceMemoryProperties g_physicalDeviceMemoryProps;

		extern QueueFamilyIndices g_queueFamilyIndices;
		extern VkQueue g_graphicsQueue;
		extern VkQueue g_computeQueue;
		extern VkQueue g_presentQueue;

		//extern ColorImage g_colorImage;
		//extern DepthImage g_depthImage;

		extern uint32_t g_currentFrame;
		extern VkSwapchainKHR g_swapChain;
		extern VkFormat g_swapChainImageFormat;
		extern VkExtent2D g_swapChainExtent;
		extern uint32_t g_swapChainImageCount;
		extern Vec<VkImage> g_swapChainImages;
		extern Vec<VkImageView> g_swapChainImageViews;
		//extern Vec<VkFramebuffer> g_swapChainFramebuffers;

		// imgui
		//extern VkRenderPass g_imguiRenderPass;
		//extern Vec<VkCommandBuffer> g_imguiCommandBuffers;
		//extern Vec<VkFramebuffer> g_imguiFrameBuffers;
		//

		extern Vec<VKFramebuffer*> g_VKOffScreenFramebuffers;
		extern VKFramebuffer* g_VKDefaultFramebuffer;

		// viewport
		extern VkExtent2D g_viewportExtent;
		//extern VkRenderPass g_viewportRenderPass;
		//extern VkPipeline g_viewportPipeline;
		//extern VkPipelineLayout g_viewportPipelineLayout;
		//extern Vec<VkCommandBuffer> g_viewportCommandBuffers;
		//extern Vec<Image> g_viewportImages;
		//extern Vec<VkFramebuffer> g_viewportFrameBuffers;
		//

		//extern VkDescriptorSetLayout g_graphicsDescriptorSetLayout;
		//extern VkDescriptorSetLayout g_computeDescriptorSetLayout;
		extern VkDescriptorPool g_descriptorPool;
		//extern Vec<VkDescriptorSet> g_graphicsDescriptorSets;
		//extern Vec<VkDescriptorSet> g_computeDescriptorSets;

		extern VkCommandPool g_commandPool;
		extern VkCommandBuffer g_currentCommandBuffer;
		//extern Vec<VkCommandBuffer> g_drawCommandBuffers;
		//extern Vec<VkCommandBuffer> g_computeCommandBuffers;

		//extern VkRenderPass g_renderPass;
		//extern VkPipeline g_graphicsPipeline;
		//extern VkPipelineLayout g_graphicsPipelineLayout;
		//extern VkPipeline g_particlePipeline;
		//extern VkPipelineLayout g_particlePipelineLayout;
		//extern VkPipeline g_computePipeline;
		//extern VkPipelineLayout g_computePipelineLayout;
	}
}