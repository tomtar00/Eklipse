#pragma once
#include <Eklipse.h>
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <Eklipse/Core/EntryPoint.h>

#include <Panels/EntitiesPanel.h>
#include <Panels/ViewPanel.h>
#include <Panels/LogsPanel.h>

#define EK_EDITOR

namespace EklipseEditor
{
	class EditorLayer : public Eklipse::Layer
	{
	public:
		~EditorLayer() {}

		void OnAttach() override;
		void OnDetach() override;
		void Update(float deltaTime) override;
	};

	class EklipseEditor : public Eklipse::Application
	{
	public:
		EklipseEditor(Eklipse::ApplicationInfo& info);

	private:
		bool GUILayerEnabled = true;
		EntitiesPanel m_entitiesPanel;
		LogsPanel m_logsPanel;
		ViewPanel m_viewPanel;
	};
}