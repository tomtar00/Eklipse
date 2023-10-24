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
		EditorLayer() = delete;
		EditorLayer(Eklipse::Scene& scene);
		~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void Render(Eklipse::Scene& scene, float deltaTime);

		void OnAPIHasInitialized(Eklipse::ApiType api);
		void OnShutdownAPI();

		inline static EditorLayer* Get() { return s_instance; }
		inline Eklipse::Scene* GetActiveScene() { return m_activeScene; }

		inline EntitiesPanel& GetEntitiesPanel() { return m_entitiesPanel; }
		inline DetailsPanel& GetDetailsPanel() { return m_detailsPanel; }
		inline LogsPanel& GetLogsPanel() { return m_logsPanel; }
		inline ViewPanel& GetViewPanel() { return m_viewPanel; }
		inline Eklipse::Camera& GetEditorCamera() { return m_editorCamera; }

		inline Eklipse::GuiLayerConfigInfo& GetGuiInfo() { return m_guiLayerCreateInfo; }
		inline void SetSelectedEntity(Eklipse::Entity entity) { m_selectedEntity = entity; }
		inline Eklipse::Entity GetSelectedEntity() { return m_selectedEntity; }
		inline void SetEntityNull() { m_selectedEntity.MarkNull(); }

		Eklipse::Ref<Eklipse::ImGuiLayer> GUI;

	private:
		inline static EditorLayer* s_instance = nullptr;

		Eklipse::GuiLayerConfigInfo m_guiLayerCreateInfo{};

		Eklipse::Ref<Eklipse::Framebuffer> m_defaultFramebuffer;
		Eklipse::Ref<Eklipse::Framebuffer> m_viewportFramebuffer;

		Eklipse::Scene* m_activeScene;
		Eklipse::Entity m_selectedEntity;
		Eklipse::Camera m_editorCamera;
		Eklipse::Transform m_editorCameraTransform;

		bool m_guiEnabled;
		EntitiesPanel	m_entitiesPanel;
		DetailsPanel	m_detailsPanel;
		LogsPanel		m_logsPanel;
		ViewPanel		m_viewPanel;
		StatsPanel		m_statsPanel;
		SettingsPanel	m_settingsPanel;
		ProfilerPanel	m_profilerPanel;
	};
}