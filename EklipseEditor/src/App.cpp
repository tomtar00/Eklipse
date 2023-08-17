#pragma once
#include <Eklipse.h>
#include <Eklipse/ImGui/ImGuiLayer.h>

#define EK_EDITOR

class HierarchyPanel : public Eklipse::ImGuiPanel
{
public:
	void OnGUI()
	{
		ImGui::Begin("Hierarchy");
		ImGui::Text("This is hierarchy");
		ImGui::End();
	}
};
class LogsPanel : public Eklipse::ImGuiPanel
{
public:
	void OnGUI()
	{
		ImGui::Begin("Logs");
		ImGui::Text("These are log messages");
		ImGui::End();
	}
};
class ViewPanel : public Eklipse::ImGuiPanel
{
public:
	void OnGUI()
	{
		ImGui::Begin("View");
		
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		Eklipse::Application::Get().m_guiLayer->GetImage(viewportPanelSize.x, viewportPanelSize.y);

		ImGui::End();
	}
};

class EditorLayer : public Eklipse::Layer 
{
public:
	~EditorLayer() {}

	void OnAttach() override
	{
		EK_INFO("Editor layer attached");
	}
	void OnDetach() override
	{
		EK_INFO("Editor layer detached");
	}
	void Update(float deltaTime) override
	{

	}
};

class EklipseEditor : public Eklipse::Application
{
public: 
	EklipseEditor(Eklipse::ApplicationInfo& info) : Application(info)
	{
		Eklipse::GuiLayerConfigInfo guiLayerCreateInfo{};
		guiLayerCreateInfo.enabled = &GUILayerEnabled;
		guiLayerCreateInfo.menuBarEnabled = true;
		guiLayerCreateInfo.dockingEnabled = true;
		guiLayerCreateInfo.dockLayouts =
		{
			{ "Hierarchy",	ImGuiDir_Left,	0.2f },
			{ "Logs",		ImGuiDir_Down,	0.3f },
			{ "View",		ImGuiDir_Right, 1.0f }
		};
		guiLayerCreateInfo.panels =
		{
			&m_hierarchyPanel, &m_logsPanel, &m_viewPanel
		};

		Eklipse::ImGuiLayer::s_ctx = ImGui::CreateContext();
		SetGuiLayer(guiLayerCreateInfo);

		PushLayer(new EditorLayer());
	}

private:
	bool GUILayerEnabled = true;
	HierarchyPanel m_hierarchyPanel;
	LogsPanel m_logsPanel;
	ViewPanel m_viewPanel;
};

Eklipse::Application* Eklipse::CreateApplication()
{
	EK_INFO("Starting editor...");

	Eklipse::ApplicationInfo info{};
	info.appName = "Eklipse Editor";
	info.windowWidth = 1280;
	info.windowHeight = 720;

	return new EklipseEditor(info);
}