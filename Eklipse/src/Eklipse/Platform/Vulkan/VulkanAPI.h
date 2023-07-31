#pragma once
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptor.h"
#include "VulkanPipeline.h"
#include "VulkanValidationLayers.h"
#include "VulkanModel.h"

#include <Eklipse/Renderer/GraphicsAPI.h>
#include <vulkan/vulkan.h>

namespace Eklipse
{
	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily;
		bool has_graphicsFamily = false;

		uint32_t presentFamily;
		bool has_presentFamily = false;

		bool isComplete();
	};

	class VulkanAPI : public GraphicsAPI
	{
	public:
		VulkanAPI();
		~VulkanAPI() override;

		void Init() override;
		void Shutdown() override;

		void DrawFrame() override;
		void WaitIdle() override;

		static VulkanAPI& Get();
		VkInstance& Instance();
		VkSurfaceKHR& Surface();
		VkQueue& GraphicsQueue();
		VkQueue& PresentQueue();
		VkSampleCountFlagBits& MsaaSamples();

		VulkanDevice& Devices();
		VulkanSwapChain& SwapChain();
		VulkanCommandPool& CommandPool();
		VulkanPipeline& Pipeline();
		VulkanDescriptorSetLayout& DescriptorLayout();
		VulkanUniformBufferPool& UniformBufferPool();
		VulkanDescriptorPool& DescriptorPool();
		VulkanValidationLayers& ValidationLayers();
		VulkanDepthImage& DepthImage();
		VulkanColorImage& ColorImage();

		VulkanModel& Model();

	public:
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		VkSampleCountFlagBits GetMaxUsableSampleCount();

	private:
		void CreateInstance();
		void CreateSurface();
		void CreateSyncObjects();
		void RecreateSwapChain();
		std::vector<const char*> GetRequiredExtensions() const;

	private:
		inline static VulkanAPI* s_instance = nullptr;

		uint32_t m_currentFrame;

		VkQueue m_graphicsQueue{};
		VkQueue m_presentQueue{};
		VkInstance m_instance{};
		VkSurfaceKHR m_surface{};
		VkSampleCountFlagBits m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		std::vector<VkSemaphore> m_imageAvailableSemaphores{};
		std::vector<VkSemaphore> m_renderFinishedSemaphores{};
		std::vector<VkFence> m_inFlightFences{};

		VulkanDevice m_device{};
		VulkanSwapChain m_swapChain{};
		VulkanCommandPool m_commandPool{};
		VulkanDescriptorSetLayout m_descriptorLayout{};
		VulkanUniformBufferPool m_uniformBufferPool{};
		VulkanDescriptorPool m_descriptorPool{};
		VulkanDepthImage m_depthImage{};
		VulkanColorImage m_colorImage{};
		VulkanPipeline m_pipeline{};
		VulkanValidationLayers m_validationLayers{};	

		VulkanModel m_model{};
	};
}