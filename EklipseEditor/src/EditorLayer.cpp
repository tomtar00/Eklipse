#include "EditorLayer.h"

namespace Editor
{
	EditorLayer::EditorLayer() : m_guiEnabled(true)
	{
		EK_ASSERT((s_instance == nullptr), "Editor layer already exists!");
		s_instance = this;
	}
	void EditorLayer::OnAttach()
	{
		m_guiLayerCreateInfo.enabled = &m_guiEnabled;
		m_guiLayerCreateInfo.menuBarEnabled = true;
		m_guiLayerCreateInfo.dockingEnabled = true;
		m_guiLayerCreateInfo.dockLayouts =
		{
			{ "Hierarchy",	ImGuiDir_Left,	0.3f },
			{ "Details",	ImGuiDir_Right, 0.5f },
			{ "Logs",		ImGuiDir_Down,	0.3f },
			{ "View",		ImGuiDir_None,  0.5f }
		};
		m_guiLayerCreateInfo.panels =
		{
			&m_entitiesPanel, &m_detailsPanel, &m_logsPanel, &m_viewPanel
		};

		EK_INFO("Editor layer attached");
	}
	void EditorLayer::OnDetach()
	{
		EK_INFO("Editor layer detached");
	}
	void EditorLayer::Update(float deltaTime)
	{
	}
}