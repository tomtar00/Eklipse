#include "precompiled.h"
#include "VkImGuiLayer.h"

#include <Eklipse/Renderer/Settings.h>

#include <Eklipse/Platform/Windows/WindowsWindow.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include "Vk.h"
#include "VkUtils.h"
#include "VkCommands.h"

namespace Eklipse
{
	namespace Vulkan
	{
		VkImGuiLayer::VkImGuiLayer(Window* window, GuiLayerConfigInfo configInfo) : 
			m_imguiPool(VK_NULL_HANDLE), Eklipse::ImGuiLayer(window, configInfo)
		{
			m_glfwWindow = dynamic_cast<WindowsWindow*>(window)->GetGlfwWindow();
			EK_ASSERT(m_glfwWindow, "Failed to get GLFW window in VK ImGui Layer!");
		}
		void VkImGuiLayer::Init()
		{
			if (s_initialized) return;
				s_initialized = true;

			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER,					1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,		1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,		1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			1000 }
			};

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000;
			pool_info.poolSizeCount = std::size(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;

			VkResult res = vkCreateDescriptorPool(g_logicalDevice, &pool_info, nullptr, &m_imguiPool);
			HANDLE_VK_RESULT(res, "IMGUI CREATE POOL");

			ImGui_ImplGlfw_InitForVulkan(m_glfwWindow, true);

			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = g_instance;
			init_info.PhysicalDevice = g_physicalDevice;
			init_info.Device = g_logicalDevice;
			init_info.Queue = g_graphicsQueue;
			init_info.DescriptorPool = m_imguiPool;
			init_info.MinImageCount = g_swapChainImageCount;
			init_info.ImageCount = g_swapChainImageCount;
			init_info.MSAASamples = RendererSettings::msaaSamples;
			init_info.CheckVkResultFn = [](VkResult res) { HANDLE_VK_RESULT(res, "IMGUI") };

			ImGui_ImplVulkan_Init(&init_info, g_renderPass);

			auto cmd = BeginSingleCommands();
			ImGui_ImplVulkan_CreateFontsTexture(cmd);
			EndSingleCommands(cmd);

			ImGui_ImplVulkan_DestroyFontUploadObjects();

			EK_CORE_DEBUG("Vulkan ImGui layer initialized");
		}
		void VkImGuiLayer::Shutdown()
		{
			if (!s_initialized) return;
			s_initialized = false;

			vkDestroyDescriptorPool(g_logicalDevice, m_imguiPool, nullptr);
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();

			EK_CORE_DEBUG("Vulkan ImGui layer shut down");
		}
		void VkImGuiLayer::NewFrame()
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		}
		void VkImGuiLayer::Draw(void* data)
		{
			if (!(*m_config.enabled)) return;

			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), (VkCommandBuffer)data);
		}
		void VkImGuiLayer::GetImage(float width, float height)
		{
			m_imageDescrSet = ImGui_ImplVulkan_AddTexture(g_viewportSamplers[g_currentFrame], g_viewportImageViews[g_currentFrame], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			ImGui::Image(m_imageDescrSet, ImVec2{width, height});
		}
	}
}