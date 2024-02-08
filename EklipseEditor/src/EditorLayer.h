#pragma once
#include <Eklipse.h>

#include <Panels/EntitiesPanel.h>
#include <Panels/ViewPanel.h>
#include <Panels/DetailsPanel.h>
#include <Panels/StatsPanel.h>
#include <Panels/SettingsPanel.h>
#include <Panels/ProfilerPanel.h>
#include <Panels/FilesPanel.h>

#include <ImGui/ImGuiExtensions.h>

namespace Eklipse
{
	struct EditorSettings
	{
		String theme = "dark";
		Path projectsPath;

		ScriptManagerSettings ScriptManagerSettings;
	};
	enum class SelectionType
	{
		NONE = 0,
		ENTITY,
		MATERIAL
	};
	struct DetailsSelectionInfo
	{
		SelectionType type;
		Entity entity;
		Material* material;
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		// === Layer ===
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float deltaTime) override;
		void OnGUI(float deltaTime) override;

		// === API Events ===
		void OnAPIHasInitialized(ApiType api);
		void OnShutdownAPI(bool quit);

		// === Project ===
		bool NewProject(const Path& path, const String& name);
		void OpenProject();
		void SaveProject();
		void SaveProjectAs();
		void SaveScene();
		void ExportProject(const ProjectExportSettings& exportSettings);
		
		// === Settings ===
		bool SerializeSettings() const;
		bool DeserializeSettings();

		// === Scene Events ===
		void OnScenePlay();
		void OnSceneStop();
		void OnScenePause();
		void OnSceneResume();

		// === Project Events ===
		void OnProjectLoaded();
		void OnProjectUnload();

		// === Getters ===
		static EditorLayer& Get();
		EditorSettings& GetSettings();
		Framebuffer* GetViewportFramebuffer() const;
		DetailsPanel& GetDetailsPanel();
		const EntitiesPanel& GetEntitiesPanel();
		const ViewPanel& GetViewPanel();
		const Camera& GetEditorCamera();
		const Ref<EditorAssetLibrary> GetAssetLibrary() const;
		bool IsPlaying() const;

		// === Setters ===
		void SetCanControlEditorCamera(bool canControl);

		// === GUI ===
		void AddDebugDrawInfo();
		void AddTeminalCommands();

		// === Selection ===
		void ClearSelection();

		// === Default Assets ===
		AssetHandle GetDefaultCubeHandle() const;
		AssetHandle GetDefaultSphereHandle() const;
		AssetHandle GetDefault3DShaderHandle() const;
		AssetHandle GetDefault2DShaderHandle() const;
		AssetHandle GetDefault3DMaterialHandle() const;
		AssetHandle GetDefault2DMaterialHandle() const;

	public:
		Ref<ImGuiLayer> GUI;
		DetailsSelectionInfo SelectionInfo{};

	private:
		inline static EditorLayer* s_instance = nullptr;
		GuiLayerConfigInfo m_guiLayerCreateInfo{};

		EditorSettings m_settings;
		Camera m_editorCamera;
		Transform m_editorCameraTransform;

		Ref<Scene> m_editorScene;
		Ref<Framebuffer> m_viewportFramebuffer;
		Ref<ScriptManager> m_scriptManager;
		Ref<EditorAssetLibrary> m_editorAssetLibrary;

		bool m_canControlEditorCamera = false;
		bool m_guiEnabled;
		EntitiesPanel	m_entitiesPanel;
		DetailsPanel	m_detailsPanel;
		ViewPanel		m_viewPanel;
		StatsPanel		m_statsPanel;
		SettingsPanel	m_settingsPanel;
		ProfilerPanel	m_profilerPanel;
		FilesPanel		m_filesPanel;
		DebugPanel		m_debugPanel;
		TerminalPanel	m_terminalPanel;

		// default asset handles
		AssetHandle m_cubeHandle;
		AssetHandle m_sphereHandle;
		AssetHandle m_shader3dHandle;
		AssetHandle m_shader2dHandle;
		AssetHandle m_material3dHandle;
		AssetHandle m_material2dHandle;
	};
}