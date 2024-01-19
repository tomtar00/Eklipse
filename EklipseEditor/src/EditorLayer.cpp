#include "EditorLayer.h"
#include <Eklipse/Scene/Components.h>
#include <Eklipse/Project/Project.h>

#include <glm/gtc/quaternion.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <filesystem>

#define EK_EDITOR_CONFIG_FILE "config.yaml"

namespace Eklipse
{
	EditorLayer::EditorLayer() : m_guiEnabled(true)
	{
		EK_ASSERT(s_instance == nullptr, "Editor layer already exists!");
		s_instance = this;

		m_editorCamera.m_farPlane = 1000.0f;
		m_editorCamera.m_nearPlane = 0.1f;
		m_editorCamera.m_fov = 45.0f;
	}
	void EditorLayer::OnAttach()
	{
		m_guiLayerCreateInfo.enabled = &m_guiEnabled;
		m_guiLayerCreateInfo.menuBarEnabled = true;
		m_guiLayerCreateInfo.dockingEnabled = true;
		m_guiLayerCreateInfo.dockLayouts =
		{
			{ "Entities",	ImGuiDir_Left,	Dir_Opposite,	0.20f },
			{ "Settings",	ImGuiDir_Down,	Dir_Same,		0.60f },
			{ "Stats",		ImGuiDir_Down,	Dir_Same,		0.50f },
			{ "Details",	ImGuiDir_Right,	Dir_Opposite,	0.25f },
			{ "Profiler",	ImGuiDir_Down,	Dir_Opposite,	0.30f },
			{ "Files",		ImGuiDir_Down,	Dir_Stack,		1.00f },
			{ "Terminal",	ImGuiDir_Right,	Dir_Same,		0.50f },
			{ "View",		ImGuiDir_Up,	Dir_Rest,		0.50f },
			{ "Debug",		ImGuiDir_Down,	Dir_Stack,		0.50f }
		};
		m_guiLayerCreateInfo.panels =
		{
			&m_entitiesPanel,
			&m_settingsPanel,
			&m_statsPanel,
			&m_detailsPanel,
			&m_profilerPanel,
			&m_viewPanel,
			&m_filesPanel,
			&Application::Get().GetDebugPanel(),
			&Application::Get().GetTerminalPanel()
		};

		m_editorScene = CreateRef<Scene>();
		m_entitiesPanel.SetContext(m_editorScene);
		Application::Get().SwitchScene(m_editorScene);

		DeserializeSettings();
		EK_TRACE("Editor layer attached");
	}
	void EditorLayer::OnDetach()
	{
		SerializeSettings();
		EK_TRACE("Editor layer detached");
	}
	void EditorLayer::OnUpdate(float deltaTime)
	{
		EK_PROFILE_NAME("Editor");

		// == CAMERA =================================
		if (m_canControlEditorCamera)
		{
			static float pitch = 0.0f;
			static float yaw = 0.0f;
			static float distance = 10.0f;
			if (Input::IsScrollingUp())
			{
				distance -= 0.5f;
			}
			if (Input::IsScrollingDown())
			{
				distance += 0.5f;
			}
			distance = glm::clamp(distance, 1.0f, 100.0f);

			static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			static glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

			if (Input::IsKeyDown(KeyCode::F))
			{
				if (GetSelection().type == SelectionType::ENTITY)
					targetPosition = GetSelection().entity.GetComponent<TransformComponent>().transform.position;
			}
			else if (Input::IsMouseButtonDown(MouseCode::Button1))
			{
				float mouseXDelta = -Input::GetMouseDeltaX();
				float mouseYDelta = Input::GetMouseDeltaY();

				pitch -= mouseYDelta * deltaTime * 100.f;
				pitch = glm::clamp(pitch, -89.0f, 89.0f);
				yaw -= mouseXDelta * deltaTime * 100.f;
			}
			else if (Input::IsMouseButtonDown(MouseCode::Button2))
			{
				float mouseXDelta = Input::GetMouseDeltaX();
				float mouseYDelta = -Input::GetMouseDeltaY();

				glm::vec3 cameraDir = glm::normalize(targetPosition - m_editorCameraTransform.position);
				glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDir, cameraUp));
				targetPosition += (mouseXDelta * cameraRight + mouseYDelta * cameraUp) * deltaTime * 5.f;
			}

			glm::vec3 cameraPosition{};
			cameraPosition.x = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
			cameraPosition.y = distance * sin(glm::radians(pitch));
			cameraPosition.z = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
			cameraPosition += targetPosition;

			m_editorCameraTransform.position = cameraPosition;
			glm::vec3 cameraDir = glm::normalize(targetPosition - m_editorCameraTransform.position);
			m_editorCameraTransform.rotation = glm::degrees(-glm::eulerAngles(glm::quatLookAt(cameraDir, cameraUp)));
		}
		// ===================================

		// == DRAW ===========================
		Renderer::BeginRenderPass(m_viewportFramebuffer);

		if (m_editorState & PLAYING)
			Application::Get().GetActiveScene()->OnSceneUpdate(deltaTime);

		if (m_editorState & EDITING)
			Renderer::RenderScene(Application::Get().GetActiveScene(), m_editorCamera, m_editorCameraTransform);
		else
			Renderer::RenderScene(Application::Get().GetActiveScene());

		Renderer::EndRenderPass(m_viewportFramebuffer);

		Renderer::BeginRenderPass(m_defaultFramebuffer);
		GUI->Render();
		Renderer::EndRenderPass(m_defaultFramebuffer);
		// ===================================
	}
	void EditorLayer::OnGUI(float deltaTime)
	{
		static bool openNewProjectPopup = false;
		static bool openExportProjectPopup = false;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItemEx("New Project", nullptr, "Ctrl+N", false, m_editorState & EDITING))
				{
					openNewProjectPopup = true;
				}
				if (ImGui::MenuItemEx("Open Project", nullptr, "Ctrl+O", false, m_editorState & EDITING))
				{
					OpenProject(); // TODO: save and load created projects list and show it in modal/window
				}
				if (ImGui::MenuItemEx("Save Project", nullptr, "Ctrl+S", false, m_editorState & EDITING))
				{
					SaveProject();
				}
				if (ImGui::MenuItemEx("Save Project As", nullptr, "Ctrl+Shift+S", false, m_editorState & EDITING))
				{
					SaveProjectAs();
				}
				if (ImGui::MenuItemEx("Save Scene", nullptr, nullptr, false, m_editorState & EDITING))
				{
					SaveScene();
				}
				if (ImGui::MenuItemEx("Export", nullptr, nullptr, false, m_editorState & EDITING))
				{
					openExportProjectPopup = true;
				}
				if (ImGui::MenuItemEx("Exit", nullptr, nullptr))
				{
					Application::Get().Close();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItemEx("Debug", nullptr, nullptr))
				{
					Application::Get().GetDebugPanel().SetVisible(true);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItemEx("Play", nullptr, "Ctrl+P", false, m_editorState & EDITING))
				{
					OnScenePlay();
				}
				if (ImGui::MenuItemEx("Pause", nullptr, "Ctrl+Shift+P", false, m_editorState & PLAYING))
				{
					OnScenePause();
				}
				if (ImGui::MenuItemEx("Resume", nullptr, "Ctrl+Shift+R", false, m_editorState & PAUSED))
				{
					OnSceneResume();
				}
				if (ImGui::MenuItemEx("Stop", nullptr, "Ctrl+Q", false, m_editorState & (PLAYING | PAUSED)))
				{
					OnSceneStop();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		// === NEW PROJECT POPUP ===
		if (openNewProjectPopup)
		{
			ImGui::OpenPopup("Create New Project");
			openNewProjectPopup = false;
		}
		if (ImGui::BeginPopupModal("Create New Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
		{
			static std::string nameBuffer;
			static Path outPath;
			ImGui::Text("Project Name");
			ImGui::SameLine();
			ImGui::InputText("##Project Name", &nameBuffer);
			ImGui::InputDir("prjlocation", "Project Location", outPath);

			if (ImGui::Button("Create"))
			{
				if (nameBuffer.empty()) { EK_WARN("Project name is empty!"); }
				else if (outPath.empty()) { EK_WARN("Project path is empty!"); }
				else
				{
					NewProject(outPath, nameBuffer);
				}

				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		// === EXPORT PROJECT POPUP ===
		if (openExportProjectPopup)
		{
			ImGui::OpenPopup("Export Project");
			openExportProjectPopup = false;
			ImGui::SetNextWindowSize({ 500, 150 });
		}
		if (ImGui::BeginPopupModal("Export Project", nullptr, ImGuiWindowFlags_NoResize))
		{
			static ProjectExportSettings exportSettings{};
			ImGui::InputDir("exportpath", "Export path", exportSettings.path);
			static int configurationIndex = 0;
			if (ImGui::Combo("Configuration", &configurationIndex, "Debug\0Release\0Dist\0"))
			{
				exportSettings.configuration = configurationIndex == 0 ? "Debug" : configurationIndex == 1 ? "Release" : "Dist";
			}
			if (ImGui::Button("Export"))
			{
				ExportProject(exportSettings);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void EditorLayer::OnAPIHasInitialized(ApiType api)
	{
		// Create default framebuffer (for ImGui)
		{
			FramebufferInfo fbInfo{};
			fbInfo.framebufferType			= FramebufferType::DEFAULT;
			fbInfo.width					= Application::Get().GetInfo().windowWidth;
			fbInfo.height					= Application::Get().GetInfo().windowHeight;
			fbInfo.numSamples				= 1;
			fbInfo.colorAttachmentInfos		= { { ImageFormat::RGBA8 } };
			fbInfo.depthAttachmentInfo		= { ImageFormat::FORMAT_UNDEFINED };

			m_defaultFramebuffer = Framebuffer::Create(fbInfo);
		}

		// Create off-screen framebuffer (for scene view)
		{
			FramebufferInfo fbInfo{};
			fbInfo.framebufferType			= FramebufferType::SCENE_VIEW;
			fbInfo.width					= GetViewPanel().GetViewportSize().x > 0 ? GetViewPanel().GetViewportSize().x : 512;
			fbInfo.height					= GetViewPanel().GetViewportSize().y > 0 ? GetViewPanel().GetViewportSize().y : 512;
			fbInfo.numSamples				= Renderer::GetSettings().GetMsaaSamples();
			fbInfo.colorAttachmentInfos		= { { ImageFormat::RGBA8 } };
			fbInfo.depthAttachmentInfo		= { ImageFormat::D24S8 };

			m_viewportFramebuffer = Framebuffer::Create(fbInfo);
		}

		GUI.reset();
		GUI = ImGuiLayer::Create(GetGuiInfo());
		Application::Get().PushOverlay(GUI);
		GUI->Init();

		OnLoadResources();
	}
	void EditorLayer::OnShutdownAPI()
	{
		Application::Get().PopOverlay(GUI);
		ClearSelection();

		GUI->Shutdown();
		m_defaultFramebuffer->Dispose();
		m_viewportFramebuffer->Dispose();

		m_defaultFramebuffer.reset();
		m_viewportFramebuffer.reset();
	}
	void EditorLayer::NewProject(const std::filesystem::path& dirPath, const std::string& name)
	{
		OnProjectUnload();

		ProjectSettings settings{};
		settings.scriptModuleSettings = &m_settings.ScriptModuleSettings;
		auto project = Project::New(settings);
		if (!Project::Exists(dirPath))
		{
			Project::SetupActive(name, dirPath);

			auto scene = Scene::New("Untitled", dirPath / project->GetConfig().startScenePath);
			Application::Get().SwitchScene(scene);

			std::filesystem::path projectFilePath = dirPath / (name + EK_PROJECT_FILE_EXTENSION);
			Project::Save(project, projectFilePath);

			m_editorScene.reset();
			m_editorScene = scene;
			m_entitiesPanel.SetContext(m_editorScene);

			OnProjectLoaded();
		}
		else
		{
			EK_ERROR("Project already exists!");
		}
	}
	void EditorLayer::OpenProject()
	{
		auto& result = OpenFileDialog({ EK_PROJECT_FILE_EXTENSION });
		if (result.type == FileDialogResultType::SUCCESS)
		{
			OnProjectUnload();

			ProjectSettings settings{};
			settings.scriptModuleSettings = &m_settings.ScriptModuleSettings;
			auto project = Project::Load(result.path, settings);

			auto scene = Scene::Load(project->GetConfig().startScenePath, Project::GetActive()->GetScriptModule()->GetLibrary());
			Application::Get().SwitchScene(scene);

			m_editorScene.reset();
			m_editorScene = scene;
			m_entitiesPanel.SetContext(m_editorScene);

			OnProjectLoaded();
		}
	}
	void EditorLayer::SaveProject()
	{
		if (!Project::GetActive()) return;

		SaveScene();
		Project::SaveActive();
	}
	void EditorLayer::SaveProjectAs()
	{
		EK_WARN("'SaveProjectAs' not implemented!");
	}
	void EditorLayer::SaveScene()
	{
		if (!Project::GetActive()) return;

		Scene::Save(m_editorScene);
	}
	void EditorLayer::ExportProject(const ProjectExportSettings& exportSettings)
	{
		if (!Project::GetActive()->Export(exportSettings))
		{
			EK_ERROR("Failed to export project!");
		}
	}
	bool EditorLayer::SerializeSettings() const
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Preferences" << YAML::Value;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Theme" << YAML::Value << m_settings.theme;
			out << YAML::EndMap;
		}

		out << YAML::Key << "ScriptModule" << YAML::Value;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "MsBuildPath" << YAML::Value << m_settings.ScriptModuleSettings.MsBuildPath.full_string();
			out << YAML::EndMap;
		}

		out << YAML::EndMap;

		std::ofstream fout(EK_EDITOR_CONFIG_FILE);
		fout << out.c_str();

		return true;
	}
	bool EditorLayer::DeserializeSettings()
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(EK_EDITOR_CONFIG_FILE);
		}
		catch (const std::exception& e)
		{
			return false;
		}

		auto preferencesNode = data["Preferences"];
		if (!preferencesNode)
			return false;

		TryDeserailize<std::string>(preferencesNode, "Theme", &m_settings.theme);

		auto& scriptModuleNode = data["ScriptModule"];
		if (scriptModuleNode)
		{
			m_settings.ScriptModuleSettings.MsBuildPath = TryDeserailize<std::string>(scriptModuleNode, "MsBuildPath", "");
		}
	}
	void EditorLayer::OnScenePlay()
	{
		if (m_editorState & PLAYING)
			return;

		m_editorState = PLAYING;
		m_canControlEditorCamera = false;

		Application::Get().SetActiveScene(Scene::Copy(m_editorScene));
		Application::Get().GetActiveScene()->OnSceneStart();

		m_entitiesPanel.SetContext(Application::Get().GetActiveScene());
		ClearSelection();

		m_scenePlayTime = Timer::Now();
	}
	void EditorLayer::OnSceneStop()
	{
		if (m_editorState & EDITING)
			return;

		m_editorState = EDITING;
		m_canControlEditorCamera = true;

		Application::Get().GetActiveScene()->OnSceneStop();
		Application::Get().SetActiveScene(m_editorScene);

		m_entitiesPanel.SetContext(Application::Get().GetActiveScene());
		ClearSelection();

		// Reload scripts if they were changed while playing
		if (Project::GetActive())
		{
			auto lastScriptReloadTime = Project::GetActive()->GetScriptModule()->GetLastStateChangeTime();
			if (lastScriptReloadTime > m_scenePlayTime)
				Scene::ReloadScripts(Application::Get().GetActiveScene());
		}
	}
	void EditorLayer::OnScenePause()
	{
		if (m_editorState & PAUSED)
			return;

		m_editorState = PAUSED;
		m_canControlEditorCamera = false;
	}
	void EditorLayer::OnSceneResume()
	{
		if (m_editorState & PLAYING)
			return;

		m_editorState = PLAYING;
		m_canControlEditorCamera = false;
	}
	void EditorLayer::OnProjectUnload()
	{
		if (Project::GetActive())
		{
			Project::GetActive()->UnloadAssets();
		}
	}
	void EditorLayer::OnLoadResources()
	{
		m_filesPanel.LoadResources();
	}
	void EditorLayer::OnProjectLoaded()
	{
		EK_ASSERT(Project::GetActive(), "Project is null!");

		ClearSelection();
		Project::GetActive()->LoadAssets();
		m_filesPanel.OnContextChanged();

		m_editorScene->ApplyAllComponents();
	}
	void EditorLayer::SetSelection(DetailsSelectionInfo info)
	{
		m_selectionInfo = info;
	}
	void EditorLayer::ClearSelection()
	{
		GetSelection().type = SelectionType::NONE;
		GetSelection().entity.MarkNull();
	}
}