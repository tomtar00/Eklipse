#include "precompiled.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include <Eklipse/Platform/Vulkan/_globals.h>
#include <Eklipse/Platform/Vulkan/VkUtils.h>
#include <Eklipse/Platform/Vulkan/VkCommads.h>

// TODO: Rework to abstract from Vulkan

namespace Eklipse
{
	void ImGuiLayer::Draw(VkCommandBuffer cmd)
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	}
	void ImGuiLayer::OnAttach()
	{
		EK_INFO("ImGui layer attached");

		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		VkResult res = vkCreateDescriptorPool(Vulkan::g_logicalDevice, &pool_info, nullptr, &m_imguiPool);
		Vulkan::HANDLE_VK_RESULT(res, "IMGUI CREATE POOL");

		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForVulkan(m_window->GetGlfwWindow(), false);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = Vulkan::g_instance;
		init_info.PhysicalDevice = Vulkan::g_physicalDevice;
		init_info.Device = Vulkan::g_logicalDevice;
		init_info.Queue = Vulkan::g_graphicsQueue;
		init_info.DescriptorPool = m_imguiPool;
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		init_info.MSAASamples = VK_SAMPLE_COUNT_8_BIT;

		ImGui_ImplVulkan_Init(&init_info, Vulkan::g_renderPass);

		auto cmd = Vulkan::BeginSingleCommands();
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		Vulkan::EndSingleCommands(cmd);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
	void ImGuiLayer::OnDetach()
	{
		EK_INFO("ImGui layer detached");

		// TODO: this is being called too late
		// first shutdown imgui, then vulkan!

		vkDestroyDescriptorPool(Vulkan::g_logicalDevice, m_imguiPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiLayer::Update(float deltaTime)
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		bool openDemo = true;
		ImGui::ShowDemoWindow(&openDemo);

		ImGui::Render();
	}
}
