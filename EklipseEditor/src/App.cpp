#pragma once
#include <Eklipse.h>
#include <iostream>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

bool g_openDemo = true;

class EditorLayer : public Eklipse::Layer 
{
public:
	~EditorLayer() {}

	void OnAttach() override
	{
		m_timeAccumulator = 0;
		m_framesAccumulator = 0;

		EK_INFO("Editor layer attached");

		//{
		//	ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;

		//  IMGUI_CHECKVERSION();
		//  ImGui::CreateContext();
		//	ImGuiIO& io = ImGui::GetIO(); (void)io;
		//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		//	ImGui::StyleColorsDark();
		//	//ImGui::StyleColorsLight();

		//	ImGui_ImplGlfw_InitForVulkan(window, true);
		//	ImGui_ImplVulkan_InitInfo init_info = {};
		//	init_info.Instance = g_Instance;
		//	init_info.PhysicalDevice = g_PhysicalDevice;
		//	init_info.Device = g_Device;
		//	init_info.QueueFamily = g_QueueFamily;
		//	init_info.Queue = g_Queue;
		//	init_info.PipelineCache = g_PipelineCache;
		//	init_info.DescriptorPool = g_DescriptorPool;
		//	init_info.Subpass = 0;
		//	init_info.MinImageCount = g_MinImageCount;
		//	init_info.ImageCount = wd->ImageCount;
		//	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		//	init_info.Allocator = g_Allocator;
		//	init_info.CheckVkResultFn = check_vk_result;
		//	ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
		//}
	}
	void OnDetach() override
	{
		EK_INFO("Editor layer detached");

		/*{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}*/
	}
	void Update(float deltaTime) override
	{
		m_timeAccumulator += deltaTime;
		m_framesAccumulator++;
		if (m_timeAccumulator >= 1)
		{
			EK_INFO("FPS: {0} FRAME TIME: {1}ms", m_framesAccumulator, deltaTime*1000.0f);

			m_timeAccumulator = 0;
			m_framesAccumulator = 0;
		}

		/*ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow(&g_openDemo);

		ImGui::Render();*/
		/*ImGui_ImplVulkan_RenderDrawData();*/
	}

private:
	float m_timeAccumulator;
	float m_framesAccumulator;
};

class EklipseEditor : public Eklipse::Application
{
public: 
	EklipseEditor(Eklipse::ApplicationInfo& info) : Application(info)
	{
		PushLayer(new EditorLayer());
	}
};

Eklipse::Application* Eklipse::CreateApplication()
{
	EK_INFO("Starting editor...");

	ApplicationInfo info{};
	info.appName = "Eklipse Editor";
	info.windowWidth = 500;
	info.windowHeight = 500;

	return new EklipseEditor(info);
}