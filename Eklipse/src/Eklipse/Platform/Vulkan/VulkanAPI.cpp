#include "precompiled.h"
#include "VulkanAPI.h"

#include <Eklipse/Core/Application.h>

#ifdef EK_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#include <Eklipse/Platform/Windows/WindowsWindow.h>
#endif

namespace Eklipse
{
	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

	bool QueueFamilyIndices::isComplete()
	{
		return has_graphicsFamily && has_presentFamily;
	}

	VulkanAPI& VulkanAPI::Get()
	{ 
		EK_ASSERT(s_instance, "Vulkan API instance not initialized!");
		return *s_instance; 
	}
	VkInstance& VulkanAPI::Instance()							{ return m_instance; }
	VkSurfaceKHR& VulkanAPI::Surface()							{ return m_surface; }
	VkQueue& VulkanAPI::GraphicsQueue()							{ return m_graphicsQueue; }
	VkQueue& VulkanAPI::PresentQueue()							{ return m_presentQueue;}
	VulkanDevice& VulkanAPI::Devices()							{ return m_device; }
	VulkanSwapChain& VulkanAPI::SwapChain()						{ return m_swapChain; }
	VulkanCommandPool& VulkanAPI::CommandPool()					{ return m_commandPool; }
	VulkanVertexBuffer& VulkanAPI::VertexBuffer()				{ return m_vertexBuffer; }
	VulkanIndexBuffer& VulkanAPI::IndexBuffer()					{ return m_indexBuffer; }
	VulkanPipeline& VulkanAPI::Pipeline()						{ return m_pipeline; }
	VulkanDescriptorSetLayout& VulkanAPI::DescriptorLayout()	{ return m_descriptorLayout; }
	VulkanUniformBufferPool& VulkanAPI::UniformBufferPool()		{ return m_uniformBufferPool; }
	VulkanDescriptorPool& VulkanAPI::DescriptorPool()			{ return m_descriptorPool; }
	VulkanValidationLayers& VulkanAPI::ValidationLayers()		{ return m_validationLayers; }
	VulkanTexture& VulkanAPI::Texture()							{ return m_texture; }

	VulkanAPI::VulkanAPI() : m_currentFrame(0), GraphicsAPI()
	{
		s_instance = this;
		m_initialized = false;
	}
	VulkanAPI::~VulkanAPI()
	{
		Shutdown();
	}

	void VulkanAPI::Init()
	{
		if (m_initialized)
		{
			EK_CORE_WARN("VulkanAPI already initialized!");
			return;
		}

		CreateInstance();
		CreateSurface();

		m_validationLayers.Init();
		m_device.Init();
		m_swapChain.InitChainViews();
		m_descriptorLayout.Init();
		m_pipeline.Init();
		m_swapChain.InitFramebuffers();
		m_commandPool.Init();
		m_texture.Load("textures/image.png");
		m_vertexBuffer.Init();
		m_indexBuffer.Init();
		m_uniformBufferPool.Init(MAX_FRAMES_IN_FLIGHT);
		m_commandPool.InitDrawBuffers(MAX_FRAMES_IN_FLIGHT);
		m_descriptorPool.Init(MAX_FRAMES_IN_FLIGHT);

		CreateSyncObjects();

		EK_CORE_INFO("Vulkan initialized");
		m_initialized = true;
	}
	void VulkanAPI::Shutdown()
	{
		if (!m_initialized)
		{
			EK_CORE_WARN("VulkanAPI has already shut down!");
			return;
		}

		m_swapChain.Shutdown();
		m_texture.Shutdown();
		m_uniformBufferPool.Shutdown();
		m_descriptorPool.Shutdown();
		m_descriptorLayout.Shutdown();
		m_vertexBuffer.Shutdown();
		m_indexBuffer.Shutdown();
		m_pipeline.Shutdown();

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(m_device.Device(), m_imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(m_device.Device(), m_renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(m_device.Device(), m_inFlightFences[i], nullptr);
		}

		m_commandPool.Shutdown();
		m_device.Shutdown();
		m_validationLayers.Shutdown();

		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyInstance(m_instance, nullptr);

		EK_CORE_INFO("Shutdown vulkan");
		m_initialized = false;
	}
	void VulkanAPI::DrawFrame()
	{
		vkWaitForFences(m_device.Device(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

		VkSwapchainKHR& swapChain = m_swapChain.SwapChain();

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_device.Device(), swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		m_uniformBufferPool.Update(m_currentFrame);

		vkResetFences(m_device.Device(), 1, &m_inFlightFences[m_currentFrame]);

		m_commandPool.RecordDrawCommandBuffer(m_currentFrame, imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandPool.DrawBuffers()[m_currentFrame];

		VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

		bool& framebufferResized = Application::Get().GetWindow()->GetData().framebufferResized;
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
		{
			framebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	void VulkanAPI::WaitIdle()
	{
		vkDeviceWaitIdle(m_device.Device());
	}

	void VulkanAPI::CreateInstance()
	{
		if (g_validationLayersEnabled && !m_validationLayers.CheckSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan Test";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Eklipse";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (g_validationLayersEnabled)
		{
			m_validationLayers.PopulateCreateInfo(debugCreateInfo, &createInfo);
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
	}
	void VulkanAPI::RecreateSwapChain()
	{
		while (Application::Get().GetWindow()->GetData().minimized)
		{
			glfwWaitEvents();
		}

		WaitIdle();

		m_swapChain.Shutdown();
		m_swapChain.InitChainViews();
		m_swapChain.InitFramebuffers();
	}
	std::vector<const char*> VulkanAPI::GetRequiredExtensions() const
	{
#ifdef EK_PLATFORM_WINDOWS
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (g_validationLayersEnabled)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions;
#else
		EK_ASSERT(false, "Platform not supported!");
#endif
	}
	void VulkanAPI::CreateSurface()
	{
#ifdef EK_PLATFORM_WINDOWS
		WindowsWindow* windowsWindow = static_cast<WindowsWindow*>(Application::Get().GetWindow());
		GLFWwindow* window = windowsWindow->GetGlfwWindow();
		bool success = glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface) == VK_SUCCESS;
		EK_ASSERT(success, "Failed to create window surface!");
#else
		EK_ASSERT(false, "Platform not supported!");
#endif
	}
	void VulkanAPI::CreateSyncObjects()
	{
		m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkDevice device = m_device.Device();
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create semaphores!");
			}
		}
	}

	VkSurfaceFormatKHR VulkanAPI::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}
	VkPresentModeKHR VulkanAPI::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D VulkanAPI::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			Application::Get().GetWindow()->GetFramebufferSize(width, height);

			VkExtent2D actualExtent =
			{
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
	VkShaderModule VulkanAPI::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_device.Device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}
	SwapChainSupportDetails VulkanAPI::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
	QueueFamilyIndices VulkanAPI::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices{};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
				indices.has_graphicsFamily = true;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
				indices.has_presentFamily = true;
			}

			i++;
		}

		return indices;
	}
}
