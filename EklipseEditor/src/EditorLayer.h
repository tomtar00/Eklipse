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

		inline EntitiesPanel& GetEntitiesPanel() { return *m_entitiesPanel.get(); }
		inline DetailsPanel& GetDetailsPanel() { return *m_detailsPanel.get(); }
		inline LogsPanel& GetLogsPanel() { return *m_logsPanel.get(); }
		inline ViewPanel& GetViewPanel() { return *m_viewPanel.get(); }
		inline Eklipse::Camera& GetEditorCamera() { return m_editorCamera; }

		inline Eklipse::GuiLayerConfigInfo& GetGuiInfo() { return m_guiLayerCreateInfo; }

	private:
		inline static EditorLayer* s_instance = nullptr;

		Eklipse::GuiLayerConfigInfo m_guiLayerCreateInfo{};

		Eklipse::Camera m_editorCamera;
		Eklipse::Transform m_editorCameraTransform;

		bool m_guiEnabled;
		Eklipse::Ref<EntitiesPanel> m_entitiesPanel;
		Eklipse::Ref<DetailsPanel> m_detailsPanel;
		Eklipse::Ref<LogsPanel> m_logsPanel;
		Eklipse::Ref<ViewPanel> m_viewPanel;
		Eklipse::Ref<StatsPanel> m_statsPanel;
		Eklipse::Ref<SettingsPanel> m_settingsPanel;
		Eklipse::Ref<ProfilerPanel> m_profilerPanel;
	};
}