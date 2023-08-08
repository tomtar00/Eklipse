#include "precompiled.h"
#include "ImGuiLayer.h"

namespace Eklipse
{
	void ImGuiLayer::OnAttach()
	{
		EK_CORE_INFO("{0} imgui layer attached", typeid(*this).name());
	}
	void ImGuiLayer::OnDetach()
	{
		ImGui::DestroyContext();

		EK_CORE_INFO("{0} imgui layer detached", typeid(*this).name());
	}
	void ImGuiLayer::Update(float deltaTime)
	{
		NewFrame();
		ImGui::NewFrame();

		for (auto& panel : m_panels)
		{
			panel->OnGUI();
		}

		ImGui::Render();	

		if (m_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImGuiLayer::AddPanel(ImGuiPanel& panel)
	{
		m_panels.push_back(&panel);
	}
}
