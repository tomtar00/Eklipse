#include "EditorLayer.h"

namespace Editor
{
	EditorLayer::EditorLayer() : m_guiEnabled(true)
	{
		EK_ASSERT(s_instance == nullptr, "Editor layer already exists!");
		s_instance = this;
	}
	void EditorLayer::OnAttach()
	{
		m_guiLayerCreateInfo.enabled = &m_guiEnabled;
		m_guiLayerCreateInfo.menuBarEnabled = true;
		m_guiLayerCreateInfo.dockingEnabled = true;
		m_guiLayerCreateInfo.dockLayouts =
		{
			{ "Hierarchy",	ImGuiDir_Left,	Eklipse::Dir_Opposite,	0.20f	},
			{ "Settings",	ImGuiDir_Down,	Eklipse::Dir_Same,		0.60f	},
			{ "Stats",		ImGuiDir_Down,	Eklipse::Dir_Same,		0.50f	},
			{ "Details",	ImGuiDir_Right,	Eklipse::Dir_Opposite,	0.25f	},
			{ "Logs",		ImGuiDir_Down,	Eklipse::Dir_Opposite,	0.20f	},
			{ "View",		ImGuiDir_None,	Eklipse::Dir_Opposite,	0.50f	}
		};
		m_guiLayerCreateInfo.panels =
		{
			&m_entitiesPanel, &m_detailsPanel, &m_logsPanel, &m_viewPanel, &m_statsPanel, &m_settingsPanel
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