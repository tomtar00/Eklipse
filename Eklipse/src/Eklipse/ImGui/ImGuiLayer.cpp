#include "precompiled.h"
#include "ImGuiLayer.h"
#include <imgui_internal.h>

namespace Eklipse
{
	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
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
	void ImGuiLayer::Update(float deltaTime)
	{
		if (!(&m_config.enabled)) return;

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

				for (auto& layout : m_config.dockLayouts)
				{
					auto dock_id = ImGui::DockBuilderSplitNode(dockspace_id, layout.dir, layout.ratio, nullptr, &dockspace_id);
					ImGui::DockBuilderDockWindow(layout.name, dock_id);
				}
				ImGui::DockBuilderFinish(dockspace_id);
			}
		}

		ImGui::End();

		for (auto& panel : m_config.panels)
		{
			panel->OnGUI();
		}

		ImGui::Render();
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
}