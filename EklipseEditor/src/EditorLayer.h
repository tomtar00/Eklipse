#pragma once
#include <Eklipse.h>

#include <Panels/EntitiesPanel.h>
#include <Panels/ViewPanel.h>
#include <Panels/LogsPanel.h>
#include <Panels/DetailsPanel.h>
#include <Panels/StatsPanel.h>
#include <Panels/SettingsPanel.h>
#include <Panels/ProfilerPanel.h>

namespace Editor
{
	class EditorLayer : public Eklipse::Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;

		inline static EditorLayer* Get() { return s_instance; }

		inline EntitiesPanel& GetEntitiesPanel() { return m_entitiesPanel; }
		inline DetailsPanel& GetDetailsPanel() { return m_detailsPanel; }
		inline LogsPanel& GetLogsPanel() { return m_logsPanel; }
		inline ViewPanel& GetViewPanel()  { return m_viewPanel; }

		inline Eklipse::GuiLayerConfigInfo& GetGuiInfo() { return m_guiLayerCreateInfo; }

	private:
		inline static EditorLayer* s_instance = nullptr;

		Eklipse::GuiLayerConfigInfo m_guiLayerCreateInfo{};

		bool m_guiEnabled;
		EntitiesPanel m_entitiesPanel;
		DetailsPanel m_detailsPanel;
		LogsPanel m_logsPanel;
		ViewPanel m_viewPanel;
		StatsPanel m_statsPanel;
		SettingsPanel m_settingsPanel;
		ProfilerPanel m_profilerPanel;
	};
}