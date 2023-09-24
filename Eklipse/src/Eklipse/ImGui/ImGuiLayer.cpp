#include "precompiled.h"
#include "ImGuiLayer.h"
#include <imgui_internal.h>

#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/OpenGL/GLImGuiLayer.h>

namespace Eklipse
{
	ImGuiLayer::ImGuiLayer(const GuiLayerConfigInfo& configInfo)
		: m_config(configInfo), m_first_time(true) {};

	void ImGuiLayer::OnAttach()
	{
		EK_ASSERT(s_ctx, "Set ImGui context (s_ctx) before pushing ImGui layer.")
		ImGui::SetCurrentContext(s_ctx);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (m_config.dockingEnabled)
		{
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}

		EK_CORE_INFO("{0} imgui layer attached", typeid(*this).name());
	}
	void ImGuiLayer::OnDetach()
	{
		ImGui::DestroyContext();

		EK_CORE_INFO("{0} imgui layer detached", typeid(*this).name());
	}
	void ImGuiLayer::OnUpdate(float deltaTime)
	{
		EK_PROFILE_NAME("GUI");

		if (!(*m_config.enabled)) return;

		NewFrame();
		ImGui::NewFrame();
		ImGuiIO& io = ImGui::GetIO();

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		if (m_config.menuBarEnabled) window_flags |= ImGuiWindowFlags_MenuBar;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

			if (m_first_time)
			{
				m_first_time = false;

				ImGui::DockBuilderRemoveNode(dockspace_id);
				ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
				ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

				ImGuiID out_opp_id = -1;
				ImGuiID node_id = dockspace_id;
				for (size_t i = 0; i < m_config.dockLayouts.size() - 1; i++)
				{
					auto& dockLayout = m_config.dockLayouts[i];

					if (dockLayout.dirType & Dir_Opposite)
					{
						dockLayout.id = ImGui::DockBuilderSplitNode(node_id, dockLayout.dir, dockLayout.ratio, nullptr, &out_opp_id);
						node_id = out_opp_id;
					}
					else if (dockLayout.dirType & Dir_Same)
					{
						auto& prevDockLayout = m_config.dockLayouts[i-1];
						dockLayout.id = ImGui::DockBuilderSplitNode(prevDockLayout.id, dockLayout.dir, dockLayout.ratio, nullptr, &out_opp_id);
						prevDockLayout.id = out_opp_id;
					}
				}
				for (int i = 0; i < m_config.dockLayouts.size() - 1; i++)
				{
					auto& dockLayout = m_config.dockLayouts[i];
					ImGui::DockBuilderDockWindow(dockLayout.name, dockLayout.id);
				}
				ImGui::DockBuilderDockWindow(m_config.dockLayouts[m_config.dockLayouts.size() - 1].name, node_id);
				ImGui::DockBuilderFinish(dockspace_id);
			}
		}

		ImGui::End();

		for (auto& panel : m_config.panels)
		{
			panel->OnGUI();
		}
	}

	void ImGuiLayer::AddPanel(ImGuiPanel& panel)
	{
		m_config.panels.push_back(&panel);
	}
	GuiLayerConfigInfo ImGuiLayer::GetConfig()
	{
		return m_config;
	}
	void ImGuiLayer::SetConfig(GuiLayerConfigInfo configInfo)
	{
		m_config = configInfo;
	}
	Ref<ImGuiLayer> ImGuiLayer::Create(const GuiLayerConfigInfo& configInfo)
	{
		auto apiType = Renderer::GetAPI();
		switch (apiType)
		{
			case ApiType::Vulkan: return CreateRef<Vulkan::VkImGuiLayer>(configInfo);
			case ApiType::OpenGL: return CreateRef<OpenGL::GLImGuiLayer>(configInfo);
		}
		EK_ASSERT(false, "API {0} not implemented for ImGui Layer creation", int(apiType));
		return nullptr;
	}
}