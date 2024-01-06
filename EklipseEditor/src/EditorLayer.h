#pragma once
#include <Eklipse.h>

#include <Panels/EntitiesPanel.h>
#include <Panels/ViewPanel.h>
#include <Panels/DetailsPanel.h>
#include <Panels/StatsPanel.h>
#include <Panels/SettingsPanel.h>
#include <Panels/ProfilerPanel.h>
#include <Panels/FilesPanel.h>

namespace Eklipse
{
	enum EditorState
	{
		EDITING = BIT(1),
		PLAYING = BIT(2),
		PAUSED	= BIT(3),
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
		Entity entity;
		Material* material;
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnGUI(float deltaTime) override;

		void OnAPIHasInitialized(ApiType api);
		void OnShutdownAPI();

		void NewProject(const std::filesystem::path& path, const std::string& name);
		void OpenProject();
		void SaveProject();
		void SaveProjectAs();
		void SaveScene();
		void ExportProject(const ProjectExportSettings& exportSettings);

		void OnScenePlay();
		void OnSceneStop();
		void OnScenePause();
		void OnSceneResume();

		void OnLoadResources();
		void OnProjectLoaded();
		void OnProjectUnload();

		inline static EditorLayer& Get() { return *s_instance; }
		inline EditorState GetEditorState() const { return m_editorState; }
		inline EntitiesPanel& GetEntitiesPanel() { return m_entitiesPanel; }
		inline DetailsPanel& GetDetailsPanel() { return m_detailsPanel; }
		inline ViewPanel& GetViewPanel() { return m_viewPanel; }
		inline Camera& GetEditorCamera() { return m_editorCamera; }
		inline GuiLayerConfigInfo& GetGuiInfo() { return m_guiLayerCreateInfo; }
		inline DetailsSelectionInfo& GetSelection() { return m_selectionInfo; }

		void SetCanControlEditorCamera(bool canControl) { m_canControlEditorCamera = canControl && (m_editorState & EditorState::EDITING); }
		void SetSelection(DetailsSelectionInfo info);
		void ClearSelection();

		Ref<ImGuiLayer> GUI;

	private:
		inline static EditorLayer* s_instance = nullptr;

		GuiLayerConfigInfo m_guiLayerCreateInfo{};
		Ref<Scene> m_editorScene;
		Ref<Framebuffer> m_defaultFramebuffer;
		Ref<Framebuffer> m_viewportFramebuffer;
		Camera m_editorCamera;
		Transform m_editorCameraTransform;
		bool m_canControlEditorCamera = false;

		EditorState m_editorState = EditorState::EDITING;
		TimePoint m_scenePlayTime;
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