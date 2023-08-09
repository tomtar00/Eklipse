#pragma once
#include <Eklipse.h>

#include <Eklipse/ImGui/ImGuiLayer.h>

class HierarchyPanel : public Eklipse::ImGuiPanel
{
public:
	void OnGUI()
	{
		ImGui::Begin("Hierarchy");
		ImGui::Text("hihi");
		ImGui::End();
	}
};
class LogsPanel : public Eklipse::ImGuiPanel
{
public:
	void OnGUI()
	{
		ImGui::Begin("Logs");
		ImGui::Text("Lolologs");
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
		guiLayerCreateInfo.dockingEnabled = true;
		guiLayerCreateInfo.dockLayouts =
		{
			{ "Hierarchy", ImGuiDir_Left, 0.2f },
			{ "Logs", ImGuiDir_Down, 0.2f }
		};
		guiLayerCreateInfo.panels =
		{
			&m_hierarchyPanel, &m_logsPanel
		};

		//PushGuiLayer(guiLayerCreateInfo);

		PushLayer(new EditorLayer());
	}

private:
	bool GUILayerEnabled = true;
	HierarchyPanel m_hierarchyPanel;
	LogsPanel m_logsPanel;
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