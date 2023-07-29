
#pragma once
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandPool.h"
#include "VulkanBuffers.h"
#include "VulkanDescriptor.h"
#include "VulkanPipeline.h"
#include "VulkanValidationLayers.h"
#include "VulkanTexture.h"

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

		VulkanDevice& Devices();
		VulkanSwapChain& SwapChain();
		VulkanCommandPool& CommandPool();
		VulkanVertexBuffer& VertexBuffer();
		VulkanIndexBuffer& IndexBuffer();
		VulkanPipeline& Pipeline();
		VulkanDescriptorSetLayout& DescriptorLayout();
		VulkanUniformBufferPool& UniformBufferPool();
		VulkanDescriptorPool& DescriptorPool();
		VulkanValidationLayers& ValidationLayers();
		VulkanTexture& Texture();
		VulkanDepthImage& DepthImage();

	public:
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

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
		std::vector<VkSemaphore> m_imageAvailableSemaphores{};
		std::vector<VkSemaphore> m_renderFinishedSemaphores{};
		std::vector<VkFence> m_inFlightFences{};

		VulkanDevice m_device{};
		VulkanSwapChain m_swapChain{};
		VulkanCommandPool m_commandPool{};

		VulkanVertexBuffer m_vertexBuffer{};
		VulkanIndexBuffer m_indexBuffer{};
		VulkanDescriptorSetLayout m_descriptorLayout{};
		VulkanUniformBufferPool m_uniformBufferPool{};
		VulkanDescriptorPool m_descriptorPool{};
		VulkanTexture m_texture{};
		VulkanDepthImage m_depthImage{};

		VulkanPipeline m_pipeline{};
		VulkanValidationLayers m_validationLayers{};	
	};
}