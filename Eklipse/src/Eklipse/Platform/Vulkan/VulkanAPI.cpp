#include "precompiled.h"
#include "Vk.h"
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
#include "VkCommands.h"
#include "VkPipeline.h"
#include "VkDescriptor.h"
#include "VkBuffers.h"
#include "VkImage.h"

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

		uint32_t			g_currentFrame;

		ColorImage			g_colorImage;
		DepthImage			g_depthImage;

		VulkanAPI::VulkanAPI() : GraphicsAPI()
		{
			s_instance = this;
			g_currentFrame = 0;
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

			// COMMON //////////////////////////////////////////

			CreateInstance();
			CreateSurface();
			SetupValidationLayers();
			PickPhysicalDevice();
			g_queueFamilyIndices = FindQueueFamilies(g_physicalDevice);
			vkGetPhysicalDeviceMemoryProperties(g_physicalDevice, &g_physicalDeviceMemoryProps);
			CreateLogicalDevice();

			VmaAllocatorCreateInfo allocatorCreateInfo = {};
			allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
			allocatorCreateInfo.physicalDevice = g_physicalDevice;
			allocatorCreateInfo.device = g_logicalDevice;
			allocatorCreateInfo.instance = g_instance;
			vmaCreateAllocator(&allocatorCreateInfo, &g_allocator);

			// GEOMETRY //////////////////////////////////////////

			int width, height;
			Application::Get().GetWindow()->GetFramebufferSize(width, height);
			g_swapChain = CreateSwapChain(width, height, g_swapChainImageCount, g_swapChainImageFormat, g_swapChainExtent, g_swapChainImages);
			CreateSwapChainImageViews(g_swapChainImageViews, g_swapChainImages);

			g_graphicsDescriptorSetLayout = CreateDescriptorSetLayout({
				{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr },
				{ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }
			});

			g_renderPass = CreateRenderPass();
			g_graphicsPipeline = CreateGraphicsPipeline(
				"shaders/vert.spv", "shaders/frag.spv",
				g_graphicsPipelineLayout, g_renderPass,
				GetVertexBindingDescription(), GetVertexAttributeDescriptions(),
				&g_graphicsDescriptorSetLayout
			);

			g_commandPool = CreateCommandPool(g_queueFamilyIndices.graphicsAndComputeFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
			CreateCommandBuffers(g_computeCommandBuffers, g_maxFramesInFlight, g_commandPool);

			g_colorImage.Setup(RendererSettings::msaaSamples);
			g_depthImage.Setup(RendererSettings::msaaSamples);

			CreateFrameBuffers(g_swapChainFramebuffers, g_swapChainImageViews, g_renderPass, g_swapChainExtent);
			g_descriptorPool = CreateDescriptorPool({
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			100	},
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	100	},
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			100	}
			}, 100);

			// PARTICLES ////////////////////////////////////////

			g_computeDescriptorSetLayout = CreateDescriptorSetLayout({
				{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr },
				{ 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr },
				{ 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr }
			});

			g_particlePipeline = CreateGraphicsPipeline(
				"shaders/particle-vert.spv", "shaders/particle-frag.spv",
				g_particlePipelineLayout, g_renderPass,
				GetParticleBindingDescription(), GetParticleAttributeDescriptions(),
				&g_graphicsDescriptorSetLayout
			);

			g_computePipeline = CreateComputePipeline(
				"shaders/particle-comp.spv", g_computePipelineLayout,
		        &g_computeDescriptorSetLayout
			);

			///////////////////////////////////////////////////

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

			// GEOMETRY //////////////////////////////////////////

			g_colorImage.Dispose();
			g_depthImage.Dispose();

			DestroyFrameBuffers(g_swapChainFramebuffers);
			DestroyImageViews(g_swapChainImageViews);
			vkDestroySwapchainKHR(g_logicalDevice, g_swapChain, nullptr);

			vkDestroyDescriptorPool(g_logicalDevice, g_descriptorPool, nullptr);
			vkDestroyDescriptorSetLayout(g_logicalDevice, g_graphicsDescriptorSetLayout, nullptr);

			vkDestroyRenderPass(g_logicalDevice, g_renderPass, nullptr);
			vkDestroyPipeline(g_logicalDevice, g_graphicsPipeline, nullptr);
			vkDestroyPipelineLayout(g_logicalDevice, g_graphicsPipelineLayout, nullptr);

			for (int i = 0; i < g_maxFramesInFlight; i++)
			{
				vkDestroySemaphore(g_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
				vkDestroySemaphore(g_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
				vkDestroyFence(g_logicalDevice, m_renderInFlightFences[i], nullptr);

				vkDestroySemaphore(g_logicalDevice, m_computeFinishedSemaphores[i], nullptr);
				vkDestroyFence(g_logicalDevice, m_computeInFlightFences[i], nullptr);
			}

			DisposeCommandPool(); // TODO: finish

			// PARTICLES //////////////////////////////////////////

			vkDestroyDescriptorSetLayout(g_logicalDevice, g_computeDescriptorSetLayout, nullptr);

			vkDestroyPipeline(g_logicalDevice, g_particlePipeline, nullptr);
			vkDestroyPipelineLayout(g_logicalDevice, g_particlePipelineLayout, nullptr);

			vkDestroyPipeline(g_logicalDevice, g_computePipeline, nullptr);
			vkDestroyPipelineLayout(g_logicalDevice, g_computePipelineLayout, nullptr);

			// COMMON /////////////////////////////////////////////

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
			// vkWaitForFences(g_logicalDevice, 1, &m_computeInFlightFences[g_currentFrame], VK_TRUE, UINT64_MAX);
			// vkResetFences(g_logicalDevice, 1, &m_computeInFlightFences[g_currentFrame]);
			// {
			// 	RecordComputeCommandBuffer();
			// }
			// VkSubmitInfo submitInfo{};
			// submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			// submitInfo.commandBufferCount = 1;
			// submitInfo.pCommandBuffers = &g_computeCommandBuffers[g_currentFrame];
			// submitInfo.signalSemaphoreCount = 1;
			// submitInfo.pSignalSemaphores = &m_computeFinishedSemaphores[g_currentFrame];
			// 
			// VkResult res = vkQueueSubmit(g_computeQueue, 1, &submitInfo, m_computeInFlightFences[g_currentFrame]);
			// HANDLE_VK_RESULT(res, "COMPUTE QUEUE SUBMIT");

			vkWaitForFences(g_logicalDevice, 1, &m_renderInFlightFences[g_currentFrame], VK_TRUE, UINT64_MAX);

			uint32_t imageIndex;
			VkResult result = vkAcquireNextImageKHR(g_logicalDevice, g_swapChain, UINT64_MAX, m_imageAvailableSemaphores[g_currentFrame], VK_NULL_HANDLE, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapChain();
				return;
			}
			EK_ASSERT(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR, "Failed to aquire swap chain image!");

			vkResetFences(g_logicalDevice, 1, &m_renderInFlightFences[g_currentFrame]);

			BeginRenderPass(imageIndex);
			{
				// DRAWING //////////////////////////////////////////////
				
				for (auto& modelAdapter : m_modelManager.m_models)
				{
					modelAdapter.Bind(g_drawCommandBuffers[g_currentFrame]);
					modelAdapter.Draw(g_drawCommandBuffers[g_currentFrame]);
				}

				Application::Get().m_guiLayer->Draw(g_drawCommandBuffers[g_currentFrame]);
				
				/////////////////////////////////////////////////////////
			}
			EndRenderPass(imageIndex);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &g_drawCommandBuffers[g_currentFrame];

			VkSemaphore waitSemaphores[] = { /*m_computeFinishedSemaphores[m_currentFrameInFlightIndex],*/ m_imageAvailableSemaphores[g_currentFrame] };
			VkPipelineStageFlags waitStages[] = { /*VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[g_currentFrame] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			result = vkQueueSubmit(g_graphicsQueue, 1, &submitInfo, m_renderInFlightFences[g_currentFrame]);
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
			else EK_ASSERT(result != VK_SUCCESS, "Failed to present swap chain image!");

			g_currentFrame = (g_currentFrame + 1) % g_maxFramesInFlight;
		}
		void VulkanAPI::DrawGUI()
		{
		}
		void VulkanAPI::OnPostLoop()
		{
			vkDeviceWaitIdle(g_logicalDevice);
		}

		float VulkanAPI::GetAspectRatio()
		{
			return (float)g_swapChainExtent.width / (float)g_swapChainExtent.height;
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
			m_renderInFlightFences.resize(g_maxFramesInFlight);

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
				VkResult res;
				res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
				HANDLE_VK_RESULT(res, "CREATE IMAGE SEMAPHORE");
				res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);
				HANDLE_VK_RESULT(res, "CREATE RENDER SEMAPHORE");
				res = vkCreateFence(device, &fenceInfo, nullptr, &m_renderInFlightFences[i]);
				HANDLE_VK_RESULT(res, "CREATE RENDER FENCE");

				res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_computeFinishedSemaphores[i]);
				HANDLE_VK_RESULT(res, "CREATE COMPUTE SEMAPHORE");
				res = vkCreateFence(device, &fenceInfo, nullptr, &m_computeInFlightFences[i]);
				HANDLE_VK_RESULT(res, "CREATE COMPUTE FENCE");
			}
		}
	}
}
