#pragma once
#include <Eklipse.h>

#include <Panels/EntitiesPanel.h>
#include <Panels/ViewPanel.h>
#include <Panels/DetailsPanel.h>
#include <Panels/StatsPanel.h>
#include <Panels/SettingsPanel.h>
#include <Panels/ProfilerPanel.h>
#include <Panels/FilesPanel.h>

namespace Editor
{
	enum class EditorState
	{
		EDITING,
		PLAYING,
		PAUSED
	};
	enum class SelectionType
	{
		NONE = 0,
		ENTITY,
		MATERIAL
	};
	struct DetailsSelectionInfo
	{
		SelectionType type = SelectionType::NONE;
		Eklipse::Entity entity;
		Eklipse::Material* material;
	};

	class EditorLayer : public Eklipse::Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnGUI(float deltaTime) override;
		void RenderActiveScene(float deltaTime);

		void OnAPIHasInitialized(Eklipse::ApiType api);
		void OnShutdownAPI();

		void NewProject(const Eklipse::Path& path, const std::string& name);
		void OpenProject();
		void SaveProject();
		void SaveProjectAs();
		void SaveScene();

		void OnScenePlay();
		void OnSceneStop();
		void OnScenePause();

		void OnLoadResources();
		void OnProjectLoaded();
		void OnProjectUnload();

		inline static EditorLayer& Get() { return *s_instance; }

		inline EntitiesPanel& GetEntitiesPanel() { return m_entitiesPanel; }
		inline DetailsPanel& GetDetailsPanel() { return m_detailsPanel; }
		inline ViewPanel& GetViewPanel() { return m_viewPanel; }
		inline Eklipse::Camera& GetEditorCamera() { return m_editorCamera; }
		inline Eklipse::GuiLayerConfigInfo& GetGuiInfo() { return m_guiLayerCreateInfo; }
		inline DetailsSelectionInfo& GetSelection() { return m_selectionInfo; }

		void SetCanControlEditorCamera(bool canControl) { m_canControlEditorCamera = canControl; }
		void SetSelection(DetailsSelectionInfo info);
		void ClearSelection();

		Eklipse::Ref<Eklipse::ImGuiLayer> GUI;

	private:
		inline static EditorLayer* s_instance = nullptr;

		Eklipse::GuiLayerConfigInfo m_guiLayerCreateInfo{};
		Eklipse::Ref<Eklipse::Scene> m_editorScene;
		Eklipse::Ref<Eklipse::Framebuffer> m_defaultFramebuffer;
		Eklipse::Ref<Eklipse::Framebuffer> m_viewportFramebuffer;
		Eklipse::Camera m_editorCamera;
		Eklipse::Transform m_editorCameraTransform;
		bool m_canControlEditorCamera = false;

		EditorState m_editorState = EditorState::EDITING;
		DetailsSelectionInfo m_selectionInfo{};

		bool m_guiEnabled;
		EntitiesPanel	m_entitiesPanel;
		DetailsPanel	m_detailsPanel;
		ViewPanel		m_viewPanel;
		StatsPanel		m_statsPanel;
		SettingsPanel	m_settingsPanel;
		ProfilerPanel	m_profilerPanel;
		FilesPanel		m_filesPanel;
	};
}