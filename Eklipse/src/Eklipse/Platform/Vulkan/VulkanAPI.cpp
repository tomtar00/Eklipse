#include "precompiled.h"
#include "VK.h"
#include "VulkanAPI.h"

#include <Eklipse/Core/Application.h>
#include <Eklipse/Renderer/Settings.h>

#ifdef EK_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>
#include <Eklipse/Platform/Windows/WindowsWindow.h>
#endif


#include "VKUtils.h"
#include "VKValidationLayers.h"
#include "VKDevice.h"
#include "VKSwapChain.h"
#include "VKCommands.h"
#include "VKPipeline.h"
#include "VKDescriptor.h"
#include "VKBuffers.h"
#include "VKImage.h"

namespace Eklipse
{
	namespace Vulkan
	{
		const uint32_t g_maxFramesInFlight = 2;

		VkInstance			g_instance = VK_NULL_HANDLE;
		VkSurfaceKHR		g_surface = VK_NULL_HANDLE;
		VmaAllocator		g_allocator = VK_NULL_HANDLE;

		VkQueue				g_graphicsQueue = VK_NULL_HANDLE;
		VkQueue				g_computeQueue = VK_NULL_HANDLE;
		VkQueue				g_presentQueue = VK_NULL_HANDLE;

		QueueFamilyIndices	g_queueFamilyIndices;

		uint32_t			g_currentFrame;
		uint32_t			g_imageIndex;

		//ColorImage			g_colorImage;
		//DepthImage			g_depthImage;

		VkCommandBuffer		g_currentCommandBuffer = VK_NULL_HANDLE;

		VKFramebuffer*		g_VKSceneFramebuffer;
		VKFramebuffer*		g_VKDefaultFramebuffer;

		VulkanAPI::VulkanAPI() : GraphicsAPI()
		{
			s_instance = this;
			g_currentFrame = 0;
		}
		VulkanAPI& VulkanAPI::Get()
		{
			return *s_instance;
		}
		bool VulkanAPI::Init()
		{
			if (m_initialized)
			{
				EK_CORE_WARN("VulkanAPI already initialized!");
				return false;
			}

			try
			{
				CreateInstance();
				CreateSurface();
				SetupValidationLayers();
				PickPhysicalDevice();
				g_queueFamilyIndices = FindQueueFamilies(g_physicalDevice);
				vkGetPhysicalDeviceMemoryProperties(g_physicalDevice, &g_physicalDeviceMemoryProps);
				CreateLogicalDevice();

				VmaAllocatorCreateInfo allocatorCreateInfo = {};
				VmaVulkanFunctions func = {};
				func.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
				func.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
				allocatorCreateInfo.pVulkanFunctions = &func;
				allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
				allocatorCreateInfo.physicalDevice = g_physicalDevice;
				allocatorCreateInfo.device = g_logicalDevice;
				allocatorCreateInfo.instance = g_instance;
				VkResult res = vmaCreateAllocator(&allocatorCreateInfo, &g_allocator);
				HANDLE_VK_RESULT(res, "CREATE VMA ALLOCATOR");


				g_commandPool = CreateCommandPool(g_queueFamilyIndices.graphicsAndComputeFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

				g_descriptorPool = CreateDescriptorPool({
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			100	},
					{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	100	},
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			100	}
					}, 100, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

				// PARTICLES ////////////////////////////////////////

				//CreateCommandBuffers(g_computeCommandBuffers, g_maxFramesInFlight, g_commandPool);
				//
				//g_computeDescriptorSetLayout = CreateDescriptorSetLayout({
				//	{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr },
				//	{ 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr },
				//	{ 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr }
				//});
				//
				//g_particlePipeline = CreateGraphicsPipeline(
				//	"shaders/particle-vert.spv", "shaders/particle-frag.spv",
				//	g_particlePipelineLayout, g_renderPass,
				//	GetParticleBindingDescription(), GetParticleAttributeDescriptions(),
				//	&g_graphicsDescriptorSetLayout
				//);
				//
				//g_computePipeline = CreateComputePipeline(
				//	"shaders/particle-comp.spv", g_computePipelineLayout,
				//    &g_computeDescriptorSetLayout
				//);

				///////////////////////////////////////////////////

				CreateSyncObjects();

				EK_CORE_INFO("Vulkan initialized");
				m_initialized = true;
				return true;
			}
			catch (const std::exception& e)
			{
				m_initialized = true;
				Shutdown();
				EK_CORE_ERROR("Vulkan initialization failed. {}", e.what());
				return false;
			}
		}
		void VulkanAPI::Shutdown()
		{
			if (!m_initialized)
			{
				EK_CORE_WARN("VulkanAPI has already shut down!");
				return;
			}

			if (g_logicalDevice)
			{
				vkDeviceWaitIdle(g_logicalDevice);
			}

			// GEOMETRY //////////////////////////////////////////

			//g_colorImage.Dispose();
			//g_depthImage.Dispose();

			// DestroyImageViews(g_swapChainImageViews);
			// vkDestroySwapchainKHR(g_logicalDevice, g_swapChain, nullptr);
			if (g_descriptorPool)
			{
				vkDestroyDescriptorPool(g_logicalDevice, g_descriptorPool, nullptr);
			}

			//DestroyFrameBuffers(g_swapChainFramebuffers);
			//
			//vkDestroyDescriptorSetLayout(g_logicalDevice, g_graphicsDescriptorSetLayout, nullptr);
			//
			//vkDestroyRenderPass(g_logicalDevice, g_renderPass, nullptr);
			//vkDestroyPipeline(g_logicalDevice, g_graphicsPipeline, nullptr);
			//vkDestroyPipelineLayout(g_logicalDevice, g_graphicsPipelineLayout, nullptr);

			for (int i = 0; i < g_maxFramesInFlight; i++)
			{
				if (m_imageAvailableSemaphores.size() && m_imageAvailableSemaphores[i])		vkDestroySemaphore(g_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
				if (m_renderFinishedSemaphores.size() && m_renderFinishedSemaphores[i])		vkDestroySemaphore(g_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
				if (m_renderInFlightFences.size() && m_renderInFlightFences[i])				vkDestroyFence(g_logicalDevice, m_renderInFlightFences[i], nullptr);
				if (m_computeFinishedSemaphores.size() && m_computeFinishedSemaphores[i])	vkDestroySemaphore(g_logicalDevice, m_computeFinishedSemaphores[i], nullptr);
				if (m_computeInFlightFences.size() && m_computeInFlightFences[i])			vkDestroyFence(g_logicalDevice, m_computeInFlightFences[i], nullptr);
			}

			//FreeCommandBuffers(g_drawCommandBuffers, g_commandPool);

			// PARTICLES //////////////////////////////////////////

			//vkDestroyDescriptorSetLayout(g_logicalDevice, g_computeDescriptorSetLayout, nullptr);
			//
			//vkDestroyPipeline(g_logicalDevice, g_particlePipeline, nullptr);
			//vkDestroyPipelineLayout(g_logicalDevice, g_particlePipelineLayout, nullptr);
			//
			//vkDestroyPipeline(g_logicalDevice, g_computePipeline, nullptr);
			//vkDestroyPipelineLayout(g_logicalDevice, g_computePipelineLayout, nullptr);
			//
			//FreeCommandBuffers(g_computeCommandBuffers, g_commandPool);

			// COMMON /////////////////////////////////////////////

			DestroyValidationLayers();
			if (g_commandPool)
			{
				vkDestroyCommandPool(g_logicalDevice, g_commandPool, nullptr);
			}

			if (g_allocator)
			{
				vmaDestroyAllocator(g_allocator);
			}

			if (g_logicalDevice)
			{
				vkDestroyDevice(g_logicalDevice, nullptr);
			}
			if (g_surface)
			{
				vkDestroySurfaceKHR(g_instance, g_surface, nullptr);
			}
			if (g_instance)
			{
				vkDestroyInstance(g_instance, nullptr);
			}

			g_VKDefaultFramebuffer = nullptr;
			g_VKSceneFramebuffer = nullptr;

			EK_CORE_INFO("Vulkan shutdown");
			m_initialized = false;
		}
		void VulkanAPI::WaitDeviceIdle()
		{
			vkDeviceWaitIdle(g_logicalDevice);
		}
		void VulkanAPI::BeginFrame()
		{
			vkWaitForFences(g_logicalDevice, 1, &m_renderInFlightFences[g_currentFrame], VK_TRUE, UINT64_MAX);
			VkResult result = vkAcquireNextImageKHR(g_logicalDevice, g_swapChain, UINT64_MAX, m_imageAvailableSemaphores[g_currentFrame], VK_NULL_HANDLE, &g_imageIndex);
			vkResetFences(g_logicalDevice, 1, &m_renderInFlightFences[g_currentFrame]);
		}
		void VulkanAPI::EndFrame()
		{
			std::array<VkSemaphore, 1> waitSemaphores = { /*m_computeFinishedSemaphores[m_currentFrameInFlightIndex],*/ m_imageAvailableSemaphores[g_currentFrame] };
			std::array<VkPipelineStageFlags, 1> waitStages = { /*VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			std::array<VkSemaphore, 1> signalSemaphores = { m_renderFinishedSemaphores[g_currentFrame] };
			std::vector<VkCommandBuffer> commandBuffers = { };

			// TODO: get rid of 'if' statments
			if (g_VKDefaultFramebuffer == g_VKSceneFramebuffer)
			{
				commandBuffers.push_back(g_VKSceneFramebuffer->GetCommandBuffer(g_currentFrame));
			}
			else
			{
				if (g_VKSceneFramebuffer != nullptr)
				{
					commandBuffers.push_back(g_VKSceneFramebuffer->GetCommandBuffer(g_currentFrame));
				}
				if (g_VKDefaultFramebuffer != nullptr)
				{
					commandBuffers.push_back(g_VKDefaultFramebuffer->GetCommandBuffer(g_currentFrame));
				}
			}

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
			submitInfo.pCommandBuffers = commandBuffers.data();
			submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
			submitInfo.pWaitSemaphores = waitSemaphores.data();
			submitInfo.pWaitDstStageMask = waitStages.data();
			submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
			submitInfo.pSignalSemaphores = signalSemaphores.data();

			VkResult result = vkQueueSubmit(g_graphicsQueue, 1, &submitInfo, m_renderInFlightFences[g_currentFrame]);
			HANDLE_VK_RESULT(result, "QUEUE SUBMIT");

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
			presentInfo.pWaitSemaphores = signalSemaphores.data();
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &g_swapChain;
			presentInfo.pImageIndices = &g_imageIndex;
			presentInfo.pResults = nullptr;

			result = vkQueuePresentKHR(g_presentQueue, &presentInfo);
			HANDLE_VK_RESULT(result, "QUEUE PRESENT");

			g_currentFrame = (g_currentFrame + 1) % g_maxFramesInFlight;
		}
		//void VulkanAPI::BeginGeometryPass()
		//{
		//	//g_currentCommandBuffer = m_viewportCommandBuffer = g_viewportCommandBuffers[g_currentFrame];
		//	//BeginRenderPass(g_viewportRenderPass, g_currentCommandBuffer, g_viewportFrameBuffers[g_viewportImageIndex], g_viewportExtent);
		//	//vkCmdBindPipeline(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_viewportPipeline);
		//}
		//void VulkanAPI::BeginGUIPass()
		//{
		//	//g_currentCommandBuffer = m_imguiCommandBuffer = g_imguiCommandBuffers[g_currentFrame];
		//	//BeginRenderPass(g_imguiRenderPass, g_currentCommandBuffer, g_imguiFrameBuffers[g_imageIndex], g_swapChainExtent);
		//	//vkCmdBindPipeline(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_graphicsPipeline);
		//}
		//void VulkanAPI::EndPass()
		//{
		//	//EndRenderPass(g_currentCommandBuffer);
		//}
		void VulkanAPI::DrawIndexed(Ref<VertexArray> vertexArray)
		{
			uint32_t numIndices = vertexArray->GetIndexBuffer()->GetCount();
			vkCmdDrawIndexed(g_currentCommandBuffer, numIndices, 1, 0, 0, 0);
		}
		void VulkanAPI::CreateInstance()
		{
			/*VkResult res;
			if (g_validationLayersEnabled)
			{
				res = CheckValidationLayersSupport();
				HANDLE_VK_RESULT(res, "VALIDATION LAYERS SUPPORT");
			}*/

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

			VkResult res = vkCreateInstance(&createInfo, nullptr, &g_instance);
			HANDLE_VK_RESULT(res, "CREATE INSTANCE");
		}
		void VulkanAPI::RecreateSwapChain()
		{
			// while (Application::Get().GetWindow()->GetData().minimized)
			// {
			// 	glfwWaitEvents();
			// }

			//vkDeviceWaitIdle(g_logicalDevice);

			//DestroyFrameBuffers(g_imguiFrameBuffers);
			//DestroyFrameBuffers(g_swapChainFramebuffers);
			//DestroyImageViews(g_swapChainImageViews);
			//vkDestroySwapchainKHR(g_logicalDevice, g_swapChain, nullptr);

			//g_depthImage.Dispose();
			//g_colorImage.Dispose();

			//int width, height;
			//Application::Get().GetWindow()->GetFramebufferSize(width, height);
			//g_swapChain = CreateSwapChain(width, height, g_swapChainImageCount, g_swapChainImageFormat, g_swapChainExtent, g_swapChainImages);
			//CreateImageViews(g_swapChainImageViews, g_swapChainImages, g_swapChainImageFormat);

			//g_colorImage.Setup((VkSampleCountFlagBits)RendererSettings::GetMsaaSamples());
			//g_depthImage.Setup((VkSampleCountFlagBits)RendererSettings::GetMsaaSamples());

			//CreateFrameBuffers(g_swapChainFramebuffers, g_swapChainImageViews, g_renderPass, g_swapChainExtent, false);
			//CreateFrameBuffers(g_imguiFrameBuffers, g_swapChainImageViews, g_imguiRenderPass, g_swapChainExtent, true);

			// uint32_t width = Application::Get().GetInfo().windowWidth;
			// uint32_t height = Application::Get().GetInfo().windowHeight;
			// g_VKDefaultFramebuffer->Resize(width, height);
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
			GLFWwindow* window = Application::Get().GetWindow()->GetGlfwWindow();
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

			for (int i = 0; i < g_maxFramesInFlight; i++)
			{
				VkResult res;
				res = vkCreateSemaphore(g_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
				HANDLE_VK_RESULT(res, "CREATE IMAGE SEMAPHORE");
				res = vkCreateSemaphore(g_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);
				HANDLE_VK_RESULT(res, "CREATE RENDER SEMAPHORE");
				res = vkCreateFence(g_logicalDevice, &fenceInfo, nullptr, &m_renderInFlightFences[i]);
				HANDLE_VK_RESULT(res, "CREATE RENDER FENCE");

				res = vkCreateSemaphore(g_logicalDevice, &semaphoreInfo, nullptr, &m_computeFinishedSemaphores[i]);
				HANDLE_VK_RESULT(res, "CREATE COMPUTE SEMAPHORE");
				res = vkCreateFence(g_logicalDevice, &fenceInfo, nullptr, &m_computeInFlightFences[i]);
				HANDLE_VK_RESULT(res, "CREATE COMPUTE FENCE");
			}
		}
	}
}