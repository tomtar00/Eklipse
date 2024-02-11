#include "precompiled.h"
#include "VK.h"
#include "VulkanAPI.h"

#include <Eklipse/Core/Application.h>

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

#include <vk_mem_alloc.h>

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
        //uint32_t			g_imageIndex;

        VkCommandBuffer		g_currentCommandBuffer = VK_NULL_HANDLE;

        //VKFramebuffer*		g_VKSceneFramebuffer;
        Vec<VKFramebuffer*> g_VKOffScreenFramebuffers{};
        VKFramebuffer*		g_VKDefaultFramebuffer;

        /*static void LogTotalStatistics(const VmaTotalStatistics& stats) {
            std::cout << "Total Statistics:" << std::endl;
            std::cout << "Block Count: " << stats.total.statistics.blockCount << std::endl;
            std::cout << "Allocation Count: " << stats.total.statistics.allocationCount << std::endl;
            std::cout << "Block Bytes: " << stats.total.statistics.blockBytes << std::endl;
            std::cout << "Allocation Bytes: " << stats.total.statistics.allocationBytes << std::endl;
        }
        static void LogDetailedStatistics(const VmaDetailedStatistics& stats, const char* title) {
            std::cout << title << " Statistics:" << std::endl;
            std::cout << "Block Count: " << stats.statistics.blockCount << std::endl;
            std::cout << "Allocation Count: " << stats.statistics.allocationCount << std::endl;
            std::cout << "Block Bytes: " << stats.statistics.blockBytes << std::endl;
            std::cout << "Allocation Bytes: " << stats.statistics.allocationBytes << std::endl;
            std::cout << "Unused Range Count: " << stats.unusedRangeCount << std::endl;
            std::cout << "Allocation Size Min: " << stats.allocationSizeMin << std::endl;
            std::cout << "Allocation Size Max: " << stats.allocationSizeMax << std::endl;
            std::cout << "Unused Range Size Min: " << stats.unusedRangeSizeMin << std::endl;
            std::cout << "Unused Range Size Max: " << stats.unusedRangeSizeMax << std::endl;
        }
        static void ListMemoryStatistics(VmaAllocator allocator) {
            VmaTotalStatistics stats;
            vmaCalculateStatistics(allocator, &stats);

            LogTotalStatistics(stats);

            for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; ++i) {
                LogDetailedStatistics(stats.memoryType[i], "Memory Type");
            }

            for (uint32_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i) {
                LogDetailedStatistics(stats.memoryHeap[i], "Memory Heap");
            }
        }*/

        VulkanAPI::VulkanAPI() : GraphicsAPI()
        {
            g_currentFrame = 0;
        }

        bool VulkanAPI::Init()
        {
            EK_CORE_PROFILE();
            if (m_initialized)
            {
                EK_CORE_WARN("VulkanAPI already initialized!");
                return true;
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
                CreateAllocator();

                g_commandPool = CreateCommandPool(g_queueFamilyIndices.graphicsAndComputeFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

                g_descriptorPool = CreateDescriptorPool(
                {
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

                CreateDefaultFramebuffer();
                CreateSyncObjects();

                EK_CORE_DBG("Vulkan initialized");
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
            EK_CORE_PROFILE();
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

            if (g_descriptorPool)
            {
                vkDestroyDescriptorPool(g_logicalDevice, g_descriptorPool, nullptr);
            }

            for (int i = 0; i < g_maxFramesInFlight; i++)
            {
                if (m_imageAvailableSemaphores.size() && m_imageAvailableSemaphores[i])		vkDestroySemaphore(g_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
                if (m_renderFinishedSemaphores.size() && m_renderFinishedSemaphores[i])		vkDestroySemaphore(g_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
                if (m_renderInFlightFences.size() && m_renderInFlightFences[i])				vkDestroyFence(g_logicalDevice, m_renderInFlightFences[i], nullptr);
                if (m_computeFinishedSemaphores.size() && m_computeFinishedSemaphores[i])	vkDestroySemaphore(g_logicalDevice, m_computeFinishedSemaphores[i], nullptr);
                if (m_computeInFlightFences.size() && m_computeInFlightFences[i])			vkDestroyFence(g_logicalDevice, m_computeInFlightFences[i], nullptr);
            }

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

            m_defaultFramebuffer->Dispose();
            m_defaultFramebuffer = nullptr;
            g_VKDefaultFramebuffer = nullptr;

            for (auto& framebuffer : g_VKOffScreenFramebuffers)
            {
                framebuffer->Dispose();
            }
            g_VKOffScreenFramebuffers.clear();

            DestroyValidationLayers();
            if (g_commandPool)
            {
                vkDestroyCommandPool(g_logicalDevice, g_commandPool, nullptr);
            }

            if (g_allocator)
            {
                //ListMemoryStatistics(g_allocator);
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

            EK_CORE_DBG("Vulkan shutdown");
            m_initialized = false;
        }
        void VulkanAPI::WaitDeviceIdle()
        {
            EK_CORE_PROFILE();
            vkDeviceWaitIdle(g_logicalDevice);
        }

        void VulkanAPI::BeginFrame()
        {
            EK_CORE_PROFILE();
            vkWaitForFences(g_logicalDevice, 1, &m_renderInFlightFences[g_currentFrame], VK_TRUE, UINT64_MAX);
            VkResult result = vkAcquireNextImageKHR(
                g_logicalDevice, g_swapChain, UINT64_MAX, 
                m_imageAvailableSemaphores[g_currentFrame], VK_NULL_HANDLE, 
                m_defaultFramebuffer->GetImageIndexPtr()
            );
            vkResetFences(g_logicalDevice, 1, &m_renderInFlightFences[g_currentFrame]);
        }
        void VulkanAPI::Submit()
        {
            EK_CORE_PROFILE();
            std::array<VkSemaphore, 1> waitSemaphores = { /*m_computeFinishedSemaphores[m_currentFrameInFlightIndex],*/ m_imageAvailableSemaphores[g_currentFrame] };
            std::array<VkPipelineStageFlags, 1> waitStages = { /*VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            std::array<VkSemaphore, 1> signalSemaphores = { m_renderFinishedSemaphores[g_currentFrame] };
            Vec<VkCommandBuffer> commandBuffers{};

            commandBuffers.push_back(m_defaultFramebuffer->GetCommandBuffer(g_currentFrame));
            for (auto& framebuffer : g_VKOffScreenFramebuffers)
            {
                commandBuffers.push_back(framebuffer->GetCommandBuffer(g_currentFrame));
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
            presentInfo.pImageIndices = m_defaultFramebuffer->GetImageIndexPtr();
            presentInfo.pResults = nullptr;

            result = vkQueuePresentKHR(g_presentQueue, &presentInfo);
            HANDLE_VK_RESULT(result, "QUEUE PRESENT");

            g_currentFrame = (g_currentFrame + 1) % g_maxFramesInFlight;
        }

        void VulkanAPI::BeginDefaultRenderPass()
        {
            EK_CORE_PROFILE();
            m_defaultFramebuffer->Bind();
        }
        void VulkanAPI::EndDefaultRenderPass()
        {
            EK_CORE_PROFILE();
            m_defaultFramebuffer->Unbind();
        }

        void VulkanAPI::OnWindowResize(uint32_t width, uint32_t height)
        {
            EK_CORE_PROFILE();
            m_defaultFramebuffer->Resize(width, height);
        }

        void VulkanAPI::DrawIndexed(Ref<VertexArray> vertexArray)
        {
            EK_CORE_PROFILE();
            uint32_t numIndices = vertexArray->GetIndexBuffer()->GetCount();
            vkCmdDrawIndexed(g_currentCommandBuffer, numIndices, 1, 0, 0, 0);
        }

        void VulkanAPI::CreateInstance()
        {
            EK_CORE_PROFILE();
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
        void VulkanAPI::CreateAllocator()
        {
            EK_CORE_PROFILE();
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
        }
        void VulkanAPI::CreateSurface()
        {
            EK_CORE_PROFILE();
#ifdef EK_PLATFORM_WINDOWS
            GLFWwindow* window = Application::Get().GetWindow()->GetGlfwWindow();
            bool success = glfwCreateWindowSurface(g_instance, window, nullptr, &g_surface) == VK_SUCCESS;
            EK_ASSERT(success, "Failed to create window surface!");
#else
            EK_ASSERT(false, "Platform not supported!");
#endif
        }
        void VulkanAPI::CreateDefaultFramebuffer()
        {
            EK_CORE_PROFILE();
            FramebufferInfo framebufferInfo{};
            framebufferInfo.isDefaultFramebuffer = true;
            framebufferInfo.width = Application::Get().GetInfo().windowWidth;
            framebufferInfo.height = Application::Get().GetInfo().windowHeight;
            framebufferInfo.numSamples = 1;
            framebufferInfo.colorAttachmentInfos = { { ImageFormat::RGBA32F } };
            framebufferInfo.depthAttachmentInfo = { ImageFormat::D24S8 };

            m_defaultFramebuffer = CreateRef<VKFramebuffer>(framebufferInfo);
        }
        void VulkanAPI::CreateSyncObjects()
        {
            EK_CORE_PROFILE();
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
        
        Vec<const char*> VulkanAPI::GetRequiredExtensions() const
        {
            EK_CORE_PROFILE();
#ifdef EK_PLATFORM_WINDOWS
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            Vec<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if (g_validationLayersEnabled)
            {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
            return extensions;
#else
            EK_ASSERT(false, "Platform not supported!");
#endif
        }
    }
}