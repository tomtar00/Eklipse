#include "precompiled.h"
#include "_globals.h"
#include "VulkanAPI.h"

#include <Eklipse/Core/Application.h>
#include <Eklipse/Renderer/Settings.h>

#ifdef EK_PLATFORM_WINDOWS
	#include <GLFW/glfw3.h>
	#include <Eklipse/Platform/Windows/WindowsWindow.h>
#endif

#include "VkUtils.h"
#include "VkValidationLayers.h"
#include "VkDevice.h"
#include "VkSwapChain.h"
#include "VkCommads.h"
#include "VkPipeline.h"
#include "VkDescriptor.h"
#include "VkBuffers.h"
#include "VkImage.h"
#include <Eklipse/ImGui/ImGuiLayer.h>

namespace Eklipse
{
	namespace Vulkan
	{
		const uint32_t g_maxFramesInFlight = 2;

		VkInstance			g_instance				= VK_NULL_HANDLE;
		VkSurfaceKHR		g_surface				= VK_NULL_HANDLE;
		VmaAllocator		g_allocator				= VK_NULL_HANDLE;

		VkQueue				g_graphicsQueue			= VK_NULL_HANDLE;
		VkQueue				g_computeQueue			= VK_NULL_HANDLE;
		VkQueue				g_presentQueue			= VK_NULL_HANDLE;

		QueueFamilyIndices	g_queueFamilyIndices;

		ColorImage			g_colorImage;
		DepthImage			g_depthImage;

		VulkanAPI::VulkanAPI() : m_currentFrameInFlightIndex(0), GraphicsAPI()
		{
			s_instance = this;
		}
		VulkanAPI::~VulkanAPI()
		{
			Shutdown();
		}
		VulkanAPI& VulkanAPI::Get()
		{
			return *s_instance;
		}
		void VulkanAPI::Init(Scene* scene)
		{
			if (m_initialized)
			{
				EK_CORE_WARN("VulkanAPI already initialized!");
				return;
			}		

			CreateInstance();
			CreateSurface();

			SetupValidationLayers();

			PickPhysicalDevice();
			g_queueFamilyIndices = FindQueueFamilies(g_physicalDevice);
			vkGetPhysicalDeviceMemoryProperties(g_physicalDevice, &g_physicalDeviceMemoryProps);

			CreateLogicalDevice();

			// TODO: Pick default graphics settings
			RendererSettings::msaaSamples = GetMaxUsableSampleCount();

			VmaAllocatorCreateInfo allocatorCreateInfo = {};
			allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
			allocatorCreateInfo.physicalDevice = g_physicalDevice;
			allocatorCreateInfo.device = g_logicalDevice;
			allocatorCreateInfo.instance = g_instance;
			vmaCreateAllocator(&allocatorCreateInfo, &g_allocator);

			SetupSwapchain();
			SetupDescriptorSetLayouts();
			SetupPipelines();
			SetupCommandPool();
			SetupCommandBuffers();

			g_colorImage.Setup(RendererSettings::msaaSamples);
			g_depthImage.Setup(RendererSettings::msaaSamples);

			SetupFramebuffers();

			SetupDescriptorPool();

			m_scene = scene;
			m_modelManager.Setup(scene);

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

			m_modelManager.Dispose();

			g_colorImage.Dispose();
			g_depthImage.Dispose();
			DisposeSwapchain();

			DisposeDescriptorPool();
			DisposeDescriptorSetLayouts();
			DisposePipelines();

			for (int i = 0; i < g_maxFramesInFlight; i++)
			{
				vkDestroySemaphore(g_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
				vkDestroySemaphore(g_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
				vkDestroyFence(g_logicalDevice, m_inFlightFences[i], nullptr);

				vkDestroySemaphore(g_logicalDevice, m_computeFinishedSemaphores[i], nullptr);
				vkDestroyFence(g_logicalDevice, m_computeInFlightFences[i], nullptr);
			}

			DisposeCommandPool();
			DisposeValidationLayers();

			vmaDestroyAllocator(g_allocator);

			DisposeDevice();
			vkDestroySurfaceKHR(g_instance, g_surface, nullptr);
			vkDestroyInstance(g_instance, nullptr);

			EK_CORE_INFO("Shutdown vulkan");
			m_initialized = false;
		}
		void VulkanAPI::DrawFrame()
		{
			// BeginComputeStage();
			// {
			// 	RecordComputeCommandBuffer(m_currentFrameInFlightIndex);
			// }
			// SubmitComputeStage();

			uint32_t imageIndex = BeginDrawStage();
			{
				BeginRenderPass(m_currentFrameInFlightIndex, imageIndex);
				// DRAWING //////////////////////////////////////////////
				{
					for (auto& modelAdapter : m_modelManager.m_models)
					{
						modelAdapter.Bind(g_drawCommandBuffers[m_currentFrameInFlightIndex]);
						modelAdapter.Draw(g_drawCommandBuffers[m_currentFrameInFlightIndex]);
					}

					//Eklipse::ImGuiLayer::Draw(g_drawCommandBuffers[m_currentFrameInFlightIndex]);
				}
				/////////////////////////////////////////////////////////
				EndRenderPass(m_currentFrameInFlightIndex, imageIndex);
			}
			SubmitDrawStage(imageIndex);

			m_currentFrameInFlightIndex = (m_currentFrameInFlightIndex + 1) % g_maxFramesInFlight;
		}
		void VulkanAPI::BeginComputeStage()
		{
			vkWaitForFences(g_logicalDevice, 1, &m_computeInFlightFences[m_currentFrameInFlightIndex], VK_TRUE, UINT64_MAX);
			vkResetFences(g_logicalDevice, 1, &m_computeInFlightFences[m_currentFrameInFlightIndex]);
		}
		void VulkanAPI::SubmitComputeStage()
		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &g_computeCommandBuffers[m_currentFrameInFlightIndex];
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &m_computeFinishedSemaphores[m_currentFrameInFlightIndex];

			if (vkQueueSubmit(g_computeQueue, 1, &submitInfo, m_computeInFlightFences[m_currentFrameInFlightIndex]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to submit compute command buffer!");
			};
		}
		uint32_t VulkanAPI::BeginDrawStage()
		{
			vkWaitForFences(g_logicalDevice, 1, &m_inFlightFences[m_currentFrameInFlightIndex], VK_TRUE, UINT64_MAX);

			uint32_t imageIndex;
			VkResult result = vkAcquireNextImageKHR(g_logicalDevice, g_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrameInFlightIndex], VK_NULL_HANDLE, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapChain();
				return imageIndex; // TODO: fix
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				throw std::runtime_error("failed to acquire swap chain image!");
			}

			vkResetFences(g_logicalDevice, 1, &m_inFlightFences[m_currentFrameInFlightIndex]);
			return imageIndex;
		}
		void VulkanAPI::SubmitDrawStage(uint32_t imageIndex)
		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &g_drawCommandBuffers[m_currentFrameInFlightIndex];

			VkSemaphore waitSemaphores[] = { /*m_computeFinishedSemaphores[m_currentFrameInFlightIndex],*/ m_imageAvailableSemaphores[m_currentFrameInFlightIndex] };
			VkPipelineStageFlags waitStages[] = { /*VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrameInFlightIndex] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			VkResult result = vkQueueSubmit(g_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrameInFlightIndex]);
			HANDLE_VK_RESULT(result, "DRAW FRAME QUEUE SUBMIT");

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = { g_swapChain };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pResults = nullptr;

			result = vkQueuePresentKHR(g_presentQueue, &presentInfo);

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
		}
		void VulkanAPI::OnPostLoop()
		{
			vkDeviceWaitIdle(g_logicalDevice);
		}

		float VulkanAPI::GetAspectRatio()
		{
			return g_swapChainExtent.width / g_swapChainExtent.height;
		}

		void VulkanAPI::CreateInstance()
		{
			VkResult res;
			if (g_validationLayersEnabled)
			{
				res = CheckValidationLayersSupport();
				HANDLE_VK_RESULT(res, "VALIDATION LAYERS SUPPORT");
			}

			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Eklipse App";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Eklipse";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_3;

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			auto extensions = GetRequiredExtensions();
			createInfo.enabledExtensionCount = extensions.size();
			createInfo.ppEnabledExtensionNames = extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			if (g_validationLayersEnabled)
			{
				PopulateInstanceCreateInfo(debugCreateInfo, &createInfo);
			}
			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.pNext = nullptr;
			}

			res = vkCreateInstance(&createInfo, nullptr, &g_instance);
			HANDLE_VK_RESULT(res, "CREATE INSTANCE");
		}
		void VulkanAPI::RecreateSwapChain()
		{
			while (Application::Get().GetWindow()->GetData().minimized)
			{
				glfwWaitEvents();
			}

			vkDeviceWaitIdle(g_logicalDevice);

			DisposeSwapchain();
			g_depthImage.Dispose();
			g_colorImage.Dispose();

			SetupSwapchain();
			g_colorImage.Setup(RendererSettings::msaaSamples);
			g_depthImage.Setup(RendererSettings::msaaSamples);
			SetupFramebuffers();
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
			bool success = glfwCreateWindowSurface(g_instance, window, nullptr, &g_surface) == VK_SUCCESS;
			EK_ASSERT(success, "Failed to create window surface!");
	#else
			EK_ASSERT(false, "Platform not supported!");
	#endif
		}
		void VulkanAPI::CreateSyncObjects()
		{
			m_imageAvailableSemaphores.resize(g_maxFramesInFlight);
			m_renderFinishedSemaphores.resize(g_maxFramesInFlight);
			m_inFlightFences.resize(g_maxFramesInFlight);

			m_computeFinishedSemaphores.resize(g_maxFramesInFlight);
			m_computeInFlightFences.resize(g_maxFramesInFlight);

			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			VkDevice device = g_logicalDevice;
			for (int i = 0; i < g_maxFramesInFlight; i++)
			{
				if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
					vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
					vkCreateFence(device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create semaphores!");
				}

				if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_computeFinishedSemaphores[i]) != VK_SUCCESS ||
					vkCreateFence(device, &fenceInfo, nullptr, &m_computeInFlightFences[i]) != VK_SUCCESS) 
				{
					throw std::runtime_error("failed to create compute synchronization objects for a frame!");
				}
			}
		}
	}
}
