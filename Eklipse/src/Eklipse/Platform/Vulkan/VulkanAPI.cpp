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

		VkInstance			g_instance				= VK_NULL_HANDLE;
		VkSurfaceKHR		g_surface				= VK_NULL_HANDLE;
		VmaAllocator		g_allocator				= VK_NULL_HANDLE;

		VkQueue				g_graphicsQueue			= VK_NULL_HANDLE;
		VkQueue				g_computeQueue			= VK_NULL_HANDLE;
		VkQueue				g_presentQueue			= VK_NULL_HANDLE;

		QueueFamilyIndices	g_queueFamilyIndices;

		uint32_t			g_currentFrame;
		uint32_t			g_imageIndex;
		uint32_t			g_viewportImageIndex;

		ColorImage			g_colorImage;
		DepthImage			g_depthImage;

		VkCommandBuffer		g_currentCommandBuffer	= VK_NULL_HANDLE;

		VulkanAPI::VulkanAPI() : GraphicsAPI()
		{
			s_instance = this;
			g_currentFrame = 0;
		}
		VulkanAPI& VulkanAPI::Get()
		{
			return *s_instance;
		}
		void VulkanAPI::Init()
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
			VmaVulkanFunctions func = {};
			func.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
			func.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
			allocatorCreateInfo.pVulkanFunctions = &func;
			allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
			allocatorCreateInfo.physicalDevice = g_physicalDevice;
			allocatorCreateInfo.device = g_logicalDevice;
			allocatorCreateInfo.instance = g_instance;
			vmaCreateAllocator(&allocatorCreateInfo, &g_allocator);

			g_commandPool = CreateCommandPool(g_queueFamilyIndices.graphicsAndComputeFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

			// GEOMETRY //////////////////////////////////////////

			int width, height;
			Application::Get().GetWindow()->GetFramebufferSize(width, height);
			g_swapChain = CreateSwapChain(width, height, g_swapChainImageCount, g_swapChainImageFormat, g_swapChainExtent, g_swapChainImages);
			CreateImageViews(g_swapChainImageViews, g_swapChainImages, g_swapChainImageFormat);

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

			CreateCommandBuffers(g_drawCommandBuffers, g_maxFramesInFlight, g_commandPool);

			g_colorImage.Setup((VkSampleCountFlagBits)RendererSettings::GetMsaaSamples());
			g_depthImage.Setup((VkSampleCountFlagBits)RendererSettings::GetMsaaSamples());

			CreateFrameBuffers(g_swapChainFramebuffers, g_swapChainImageViews, g_renderPass, g_swapChainExtent, false);
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
		}
		void VulkanAPI::Shutdown()
		{
			if (!m_initialized)
			{
				EK_CORE_WARN("VulkanAPI has already shut down!");
				return;
			}

			vkDeviceWaitIdle(g_logicalDevice);

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

			FreeCommandBuffers(g_drawCommandBuffers, g_commandPool);

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
			vkDestroyCommandPool(g_logicalDevice, g_commandPool, nullptr);

			vmaDestroyAllocator(g_allocator);

			DisposeDevice();
			vkDestroySurfaceKHR(g_instance, g_surface, nullptr);
			vkDestroyInstance(g_instance, nullptr);

			EK_CORE_INFO("Vulkan shutdown ");
			m_initialized = false;
		}
		void VulkanAPI::BeginFrame()
		{
			vkWaitForFences(g_logicalDevice, 1, &m_renderInFlightFences[g_currentFrame], VK_TRUE, UINT64_MAX);

			VkResult result = vkAcquireNextImageKHR(g_logicalDevice, g_swapChain, UINT64_MAX, m_imageAvailableSemaphores[g_currentFrame], VK_NULL_HANDLE, &g_imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RecreateSwapChain();
				return;
			}
			EK_ASSERT((result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR), "Failed to aquire swap chain image!");

			vkResetFences(g_logicalDevice, 1, &m_renderInFlightFences[g_currentFrame]);
		}
		void VulkanAPI::EndFrame()
		{
			std::array<VkSemaphore, 1> waitSemaphores = { /*m_computeFinishedSemaphores[m_currentFrameInFlightIndex],*/ m_imageAvailableSemaphores[g_currentFrame] };
			std::array<VkPipelineStageFlags, 1> waitStages = { /*VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			std::array<VkSemaphore, 1> signalSemaphores = { m_renderFinishedSemaphores[g_currentFrame] };
			std::array<VkCommandBuffer, 2> commandBuffers = { /*drawCommandBuffer,*/ m_viewportCommandBuffer, m_imguiCommandBuffer };

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = commandBuffers.size();
			submitInfo.pCommandBuffers = commandBuffers.data();
			submitInfo.waitSemaphoreCount = waitSemaphores.size();
			submitInfo.pWaitSemaphores = waitSemaphores.data();
			submitInfo.pWaitDstStageMask = waitStages.data();
			submitInfo.signalSemaphoreCount = signalSemaphores.size();
			submitInfo.pSignalSemaphores = signalSemaphores.data();

			VkResult result = vkQueueSubmit(g_graphicsQueue, 1, &submitInfo, m_renderInFlightFences[g_currentFrame]);
			HANDLE_VK_RESULT(result, "DRAW FRAME QUEUE SUBMIT");

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = signalSemaphores.size();
			presentInfo.pWaitSemaphores = signalSemaphores.data();
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &g_swapChain;
			presentInfo.pImageIndices = &g_imageIndex;
			presentInfo.pResults = nullptr;

			result = vkQueuePresentKHR(g_presentQueue, &presentInfo);

			bool& framebufferResized = Application::Get().GetWindow()->GetData().framebufferResized;
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
			{
				framebufferResized = false;
				RecreateSwapChain();
			}
			else EK_ASSERT(result == VK_SUCCESS, "Failed to present swap chain image!");

			g_currentFrame = (g_currentFrame + 1) % g_maxFramesInFlight;
		}
		void VulkanAPI::BeginGeometryPass()
		{
			g_currentCommandBuffer = m_viewportCommandBuffer = g_viewportCommandBuffers[g_currentFrame];
			BeginRenderPass(g_viewportRenderPass, g_currentCommandBuffer, g_viewportFrameBuffers[g_viewportImageIndex], g_viewportExtent);
			vkCmdBindPipeline(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_viewportPipeline);
		}
		void VulkanAPI::BeginGUIPass()
		{
			g_currentCommandBuffer = m_imguiCommandBuffer = g_imguiCommandBuffers[g_currentFrame];
			BeginRenderPass(g_imguiRenderPass, g_currentCommandBuffer, g_imguiFrameBuffers[g_imageIndex], g_swapChainExtent);
			vkCmdBindPipeline(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_graphicsPipeline);
		}
		void VulkanAPI::EndPass()
		{
			EndRenderPass(g_currentCommandBuffer);
		}
		void VulkanAPI::DrawIndexed(Ref<VertexArray> vertexArray)
		{
			//entity.m_uniformBuffer->SetData(&entity.m_ubo, sizeof(entity.m_ubo));

			// TODO: add maybe some unsorted_map Entity->VkDescriptorSet and select descriptorSet from there
			//VkDescriptorSet descriptorSet = m_descriptorSet;
			//vkCmdBindDescriptorSets(g_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_graphicsPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
			uint32_t numIndices = vertexArray->GetIndexBuffer()->GetCount();
			vkCmdDrawIndexed(g_currentCommandBuffer, numIndices, 1, 0, 0, 0);
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

			DestroyFrameBuffers(g_imguiFrameBuffers);
			DestroyFrameBuffers(g_swapChainFramebuffers);
			DestroyImageViews(g_swapChainImageViews);
			vkDestroySwapchainKHR(g_logicalDevice, g_swapChain, nullptr);

			g_depthImage.Dispose();
			g_colorImage.Dispose();

			int width, height;
			Application::Get().GetWindow()->GetFramebufferSize(width, height);
			g_swapChain = CreateSwapChain(width, height, g_swapChainImageCount, g_swapChainImageFormat, g_swapChainExtent, g_swapChainImages);
			CreateImageViews(g_swapChainImageViews, g_swapChainImages, g_swapChainImageFormat);

			g_colorImage.Setup((VkSampleCountFlagBits)RendererSettings::GetMsaaSamples());
			g_depthImage.Setup((VkSampleCountFlagBits)RendererSettings::GetMsaaSamples());

			CreateFrameBuffers(g_swapChainFramebuffers, g_swapChainImageViews, g_renderPass, g_swapChainExtent, false);
			CreateFrameBuffers(g_imguiFrameBuffers, g_swapChainImageViews, g_imguiRenderPass, g_swapChainExtent, true);

			Application::Get().m_guiLayer->ResizeViewport(512, 512);
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