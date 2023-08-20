#pragma once
#include "EditorApp.h"

namespace EklipseEditor
{
	void EditorLayer::OnAttach()
	{
		EK_INFO("Editor layer attached");
	}
	void EditorLayer::OnDetach()
	{
		EK_INFO("Editor layer detached");
	}
	void EditorLayer::Update(float deltaTime)
	{
	}

	EklipseEditor::EklipseEditor(Eklipse::ApplicationInfo& info) : Application(info)
	{
		Eklipse::GuiLayerConfigInfo guiLayerCreateInfo{};
		guiLayerCreateInfo.enabled = &GUILayerEnabled;
		guiLayerCreateInfo.menuBarEnabled = true;
		guiLayerCreateInfo.dockingEnabled = true;
		guiLayerCreateInfo.dockLayouts =
		{
			{ "Hierarchy",	ImGuiDir_Left,	0.2f },
			{ "Logs",		ImGuiDir_Down,	0.3f },
			{ "View",		ImGuiDir_None,  0.5f }
		};
		guiLayerCreateInfo.panels =
		{
			&m_entitiesPanel, &m_logsPanel, &m_viewPanel
		};

		Eklipse::ImGuiLayer::s_ctx = ImGui::CreateContext();
		SetGuiLayer(guiLayerCreateInfo);

		PushLayer(new EditorLayer());
	}
}

Eklipse::Application* Eklipse::CreateApplication()
{
	EK_INFO("Starting editor...");

	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse Editor";
	info.windowWidth = 1600;
	info.windowHeight = 900;

	return new EklipseEditor::EklipseEditor(info);
}