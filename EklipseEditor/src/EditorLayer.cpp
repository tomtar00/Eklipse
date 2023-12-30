#include "EditorLayer.h"
#include <Eklipse/Renderer/Settings.h>
#include <Eklipse/Scene/Components.h>
#include <Eklipse/Project/Project.h>

#include <glm/gtc/quaternion.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <filesystem>
#include <nfd.h>

namespace Editor
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
			{ "Entities",	ImGuiDir_Left,	Eklipse::Dir_Opposite,	0.20f },
			{ "Settings",	ImGuiDir_Down,	Eklipse::Dir_Same,		0.60f },
			{ "Stats",		ImGuiDir_Down,	Eklipse::Dir_Same,		0.50f },
			{ "Details",	ImGuiDir_Right,	Eklipse::Dir_Opposite,	0.25f },
			{ "Profiler",	ImGuiDir_Down,	Eklipse::Dir_Opposite,	0.30f },
			{ "Files",		ImGuiDir_Down,	Eklipse::Dir_Stack,		1.00f },
			{ "Terminal",	ImGuiDir_Right,	Eklipse::Dir_Same,		0.50f },
			{ "View",		ImGuiDir_Up,	Eklipse::Dir_Rest,		0.50f },
			{ "Debug",		ImGuiDir_Down,	Eklipse::Dir_Stack,		0.50f }
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
			&Eklipse::Application::Get().GetDebugPanel(),
			&Eklipse::Application::Get().GetTerminalPanel()
		};

		m_editorScene = Eklipse::CreateRef<Eklipse::Scene>();
		m_entitiesPanel.SetContext(m_editorScene);
		Eklipse::Application::Get().SwitchScene(m_editorScene);

		EK_INFO("Editor layer attached");
	}
	void EditorLayer::OnDetach()
	{
		EK_INFO("Editor layer detached");
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
			if (Eklipse::Input::IsScrollingUp())
			{
				distance -= 0.5f;
			}
			if (Eklipse::Input::IsScrollingDown())
			{
				distance += 0.5f;
			}
			distance = glm::clamp(distance, 1.0f, 100.0f);

			static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
			static glm::vec3 targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);

			// TODO: check if input was started in view window

			if (Eklipse::Input::IsKeyDown(Eklipse::KeyCode::F))
			{
				if (GetSelection().type == SelectionType::ENTITY)
					targetPosition = GetSelection().entity.GetComponent<Eklipse::TransformComponent>().transform.position;
			}
			else if (Eklipse::Input::IsMouseButtonDown(Eklipse::MouseCode::Button1))
			{
				float mouseXDelta = -Eklipse::Input::GetMouseDeltaX();
				float mouseYDelta = Eklipse::Input::GetMouseDeltaY();

				pitch -= mouseYDelta * deltaTime * 100.f;
				pitch = glm::clamp(pitch, -89.0f, 89.0f);
				yaw -= mouseXDelta * deltaTime * 100.f;
			}
			else if (Eklipse::Input::IsMouseButtonDown(Eklipse::MouseCode::Button2))
			{
				float mouseXDelta = Eklipse::Input::GetMouseDeltaX();
				float mouseYDelta = -Eklipse::Input::GetMouseDeltaY();

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
	}
	void EditorLayer::OnGUI(float deltaTime)
	{
		static bool openNewProjectPopup = false;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItemEx("New Project", nullptr, "Ctrl+N"))
				{
					openNewProjectPopup = true;
				}
				if (ImGui::MenuItemEx("Open Project", nullptr, "Ctrl+O"))
				{
					OpenProject(); // TODO: save and load created projects list and show it in modal/window
				}
				if (ImGui::MenuItemEx("Save Project", nullptr, "Ctrl+S"))
				{
					SaveProject();
				}
				if (ImGui::MenuItemEx("Save Project As", nullptr, "Ctrl+Shift+S"))
				{
					SaveProjectAs();
				}
				if (ImGui::MenuItemEx("Save Scene", nullptr, nullptr))
				{
					SaveScene();
				}
				if (ImGui::MenuItemEx("Exit", nullptr, nullptr))
				{
					Eklipse::Application::Get().Close();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItemEx("Debug", nullptr, nullptr))
				{
					Eklipse::Application::Get().GetDebugPanel().SetVisible(true);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItemEx("Play", nullptr, "Ctrl+P", false, m_editorState & EditorState::EDITING))
				{
					OnScenePlay();
				}
				if (ImGui::MenuItemEx("Pause", nullptr, "Ctrl+Shift+P", false, m_editorState & EditorState::PLAYING))
				{
					OnScenePause();
				}
				if (ImGui::MenuItemEx("Resume", nullptr, "Ctrl+Shift+R", false, m_editorState & EditorState::PAUSED))
				{
					OnSceneResume();
				}
				if (ImGui::MenuItemEx("Stop", nullptr, "Ctrl+Q", false, m_editorState & (EditorState::PLAYING | EditorState::PAUSED)))
				{
					OnSceneStop();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (openNewProjectPopup)
		{
			ImGui::OpenPopup("Create New Project");
			openNewProjectPopup = false;
		}
		if (ImGui::BeginPopupModal("Create New Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
		{
			static std::string nameBuffer;
			static char* outPath = nullptr;
			ImGui::Text("Project Name: ");
			ImGui::SameLine();
			ImGui::InputText("##Project Name", &nameBuffer);
			ImGui::Text("Project Location: ");
			ImGui::SameLine();
			ImGui::Text(outPath == nullptr ? "" : outPath);
			ImGui::SameLine();
			if (ImGui::Button("Browse"))
			{
				nfdresult_t result = NFD_PickFolder(nullptr, &outPath);
				//if (result == NFD_CANCEL) return;

				EK_ASSERT(result != NFD_ERROR, "Failed to open directory! {0}", NFD_GetError());
			}
			if (ImGui::Button("Create"))
			{
				if (nameBuffer.empty()) { EK_WARN("Project name is empty!"); }
				else if (outPath == nullptr) { EK_WARN("Project path is empty!"); }
				else
				{
					NewProject(outPath, nameBuffer);
				}

				if (outPath != nullptr)
					free(outPath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				if (outPath != nullptr)
					free(outPath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void EditorLayer::DrawFrame(float deltaTime)
	{
		EK_PROFILE();

		if (m_editorState & EditorState::PLAYING)
			Eklipse::Application::Get().GetActiveScene()->OnSceneUpdate(deltaTime);
		
		Eklipse::Renderer::BeginFrame();

		// === Record scene framebuffer
		Eklipse::Renderer::BeginRenderPass(m_viewportFramebuffer);
		if (m_editorState & EditorState::EDITING)
			Eklipse::Renderer::RenderScene(Eklipse::Application::Get().GetActiveScene(), m_editorCamera, m_editorCameraTransform);
		else
			Eklipse::Renderer::RenderScene(Eklipse::Application::Get().GetActiveScene());
		Eklipse::Renderer::EndRenderPass(m_viewportFramebuffer);
		// ============================

		// === Record ImGui framebuffer
		Eklipse::Renderer::BeginRenderPass(m_defaultFramebuffer);
		GUI->Render();
		Eklipse::Renderer::EndRenderPass(m_defaultFramebuffer);
		// ============================

		Eklipse::Renderer::Submit();
	}
	void EditorLayer::OnAPIHasInitialized(Eklipse::ApiType api)
	{
		// Create default framebuffer (for ImGui)
		{
			Eklipse::FramebufferInfo fbInfo{};
			fbInfo.framebufferType			= Eklipse::FramebufferType::DEFAULT;
			fbInfo.width					= Eklipse::Application::Get().GetInfo().windowWidth;
			fbInfo.height					= Eklipse::Application::Get().GetInfo().windowHeight;
			fbInfo.numSamples				= 1;
			fbInfo.colorAttachmentInfos		= { { Eklipse::ImageFormat::RGBA8 } };
			fbInfo.depthAttachmentInfo		= { Eklipse::ImageFormat::FORMAT_UNDEFINED };

			m_defaultFramebuffer = Eklipse::Framebuffer::Create(fbInfo);
		}

		// Create off-screen framebuffer (for scene view)
		{
			Eklipse::FramebufferInfo fbInfo{};
			fbInfo.framebufferType			= Eklipse::FramebufferType::SCENE_VIEW;
			fbInfo.width					= GetViewPanel().GetViewportSize().x > 0 ? GetViewPanel().GetViewportSize().x : 512;
			fbInfo.height					= GetViewPanel().GetViewportSize().y > 0 ? GetViewPanel().GetViewportSize().y : 512;
			fbInfo.numSamples				= Eklipse::RendererSettings::GetMsaaSamples();
			fbInfo.colorAttachmentInfos		= { { Eklipse::ImageFormat::RGBA8 } };
			fbInfo.depthAttachmentInfo		= { Eklipse::ImageFormat::D24S8 };

			m_viewportFramebuffer = Eklipse::Framebuffer::Create(fbInfo);
		}

		GUI.reset();
		GUI = Eklipse::ImGuiLayer::Create(GetGuiInfo());
		Eklipse::Application::Get().PushOverlay(GUI);
		GUI->Init();

		OnLoadResources();
	}
	void EditorLayer::OnShutdownAPI()
	{
		Eklipse::Application::Get().PopOverlay(GUI);
		ClearSelection();

		GUI->Shutdown();
		m_defaultFramebuffer->Dispose();
		m_viewportFramebuffer->Dispose();

		m_defaultFramebuffer.reset();
		m_viewportFramebuffer.reset();
	}
	void EditorLayer::NewProject(const Eklipse::Path& dirPath, const std::string& name)
	{
		OnProjectUnload();

		auto project = Eklipse::Project::New();
		if (!Eklipse::Project::Exists(dirPath))
		{
			// Create project directories and copy neccessary files
			Eklipse::Project::SetupActive(name, dirPath);

			// Create default scene
			auto scene = Eklipse::Scene::New("Untitled", project->GetConfig().startScenePath);

			// Save project
			std::filesystem::path projectFilePath = dirPath.path() / (name + EK_PROJECT_FILE_EXTENSION);
			Eklipse::Project::Save(project, projectFilePath);

			Eklipse::Application::Get().SwitchScene(scene);
			OnProjectLoaded();
		}
		else
		{
			EK_ERROR("Project already exists!");
		}
	}
	void EditorLayer::OpenProject()
	{
		nfdchar_t* outPath = nullptr;
		nfdresult_t result = NFD_OpenDialog("ekproj", nullptr, &outPath);
		if (result == NFD_CANCEL) return;

		EK_ASSERT(result == NFD_OKAY, "Failed to open project file! {0}", NFD_GetError());

		OnProjectUnload();
		auto project = Eklipse::Project::Load(outPath);
		auto scene = Eklipse::Scene::Load(project->GetConfig().startScenePath);

		Eklipse::Application::Get().SwitchScene(scene);
		m_editorScene.reset();
		m_editorScene = scene;
		m_entitiesPanel.SetContext(m_editorScene);

		OnProjectLoaded();
		free(outPath);
	}
	void EditorLayer::SaveProject()
	{
		SaveScene();
		Eklipse::Project::SaveActive();
	}
	void EditorLayer::SaveProjectAs()
	{
		EK_WARN("'SaveProjectAs' not implemented!");
	}
	void EditorLayer::SaveScene()
	{
		Eklipse::Scene::Save(m_editorScene);
	}
	void EditorLayer::OnScenePlay()
	{
		if (m_editorState & EditorState::PLAYING)
			return;

		m_editorState = EditorState::PLAYING;
		m_canControlEditorCamera = false;

		Eklipse::Application::Get().SetActiveScene(Eklipse::Scene::Copy(m_editorScene));
		Eklipse::Application::Get().GetActiveScene()->OnSceneStart();

		m_entitiesPanel.SetContext(Eklipse::Application::Get().GetActiveScene());
		ClearSelection();
	}
	void EditorLayer::OnSceneStop()
	{
		if (m_editorState & EditorState::EDITING)
			return;

		m_editorState = EditorState::EDITING;
		m_canControlEditorCamera = true;

		Eklipse::Application::Get().GetActiveScene()->OnSceneStop();
		Eklipse::Application::Get().SetActiveScene(m_editorScene);

		m_entitiesPanel.SetContext(Eklipse::Application::Get().GetActiveScene());
		ClearSelection();
	}
	void EditorLayer::OnScenePause()
	{
		if (m_editorState & EditorState::PAUSED)
			return;

		m_editorState = EditorState::PAUSED;
		m_canControlEditorCamera = false;
	}
	void EditorLayer::OnSceneResume()
	{
		if (m_editorState & EditorState::PLAYING)
			return;

		m_editorState = EditorState::PLAYING;
		m_canControlEditorCamera = false;
	}
	void EditorLayer::OnProjectUnload()
	{
		Eklipse::Renderer::WaitDeviceIdle();
		if (Eklipse::Project::GetActive())
			Eklipse::Project::GetActive()->UnloadAssets();
	}
	void EditorLayer::OnLoadResources()
	{
		m_filesPanel.LoadResources();
	}
	void EditorLayer::OnProjectLoaded()
	{
		ClearSelection();
		Eklipse::Project::GetActive()->LoadAssets();
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