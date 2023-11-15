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
	EditorLayer::EditorLayer(Eklipse::Ref<Eklipse::Scene> scene) : m_guiEnabled(true)
	{
		EK_ASSERT(s_instance == nullptr, "Editor layer already exists!");
		s_instance = this;

		m_editorCamera.m_farPlane = 1000.0f;
		m_editorCamera.m_nearPlane = 0.1f;
		m_editorCamera.m_fov = 45.0f;

		m_assetLibrary = Eklipse::CreateRef<Eklipse::AssetLibrary>();
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
			{ "Logs",		ImGuiDir_Down,	Eklipse::Dir_Opposite,	0.30f },
			{ "Profiler",	ImGuiDir_Down,	Eklipse::Dir_Stack,		1.00f },
			{ "Files",		ImGuiDir_Down,	Eklipse::Dir_Stack,		1.00f },	
			{ "View",		ImGuiDir_None,	Eklipse::Dir_Opposite,	0.50f }
		};
		m_guiLayerCreateInfo.panels =
		{
			&m_entitiesPanel,
			&m_settingsPanel,
			&m_statsPanel,
			&m_detailsPanel,
			&m_logsPanel,
			&m_profilerPanel,
			&m_viewPanel,
			&m_filesPanel
		};

		EK_INFO("Editor layer attached");
	}
	void EditorLayer::OnDetach()
	{
		EK_INFO("Editor layer detached");
	}
	void EditorLayer::OnUpdate(float deltaTime)
	{
		EK_PROFILE_NAME("Editor");

		// =================================== CAMERA
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
			if (GetSelection().type == SelectionType::Entity)
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

				EK_ASSERT(result == NFD_OKAY, "Failed to open directory! {0}", NFD_GetError());
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
	void EditorLayer::Render(Eklipse::Ref<Eklipse::Scene> scene, float deltaTime)
	{
		EK_PROFILE();
		
		Eklipse::Renderer::BeginFrame(m_editorCamera, m_editorCameraTransform);

		// Record scene framebuffer
		Eklipse::Renderer::BeginRenderPass(m_viewportFramebuffer);
		Eklipse::Renderer::RenderScene(scene);
		Eklipse::Renderer::EndRenderPass(m_viewportFramebuffer);

		// Record ImGui framebuffer
		Eklipse::Renderer::BeginRenderPass(m_defaultFramebuffer);
		GUI->Render();
		Eklipse::Renderer::EndRenderPass(m_defaultFramebuffer);

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

		m_assetLibrary->Unload();
	}
	void EditorLayer::NewProject(const Eklipse::Path& path, const std::string& name)
	{
		OnProjectUnload();

		auto project = Eklipse::Project::New();
		if (!Eklipse::Project::Exists(path))
		{
			// === Create config
			std::string defaultSceneName = "Unititled";
			project->GetConfig().name = name;
			project->GetConfig().assetsDirectoryPath = path.path() / "Assets";
			project->GetConfig().startScenePath = project->GetConfig().assetsDirectoryPath / "Scenes" / (defaultSceneName + EK_SCENE_FILE_EXTENSION);

			// === Create default scene
			std::filesystem::create_directories(project->GetConfig().assetsDirectoryPath / "Scenes");
			auto scene = Eklipse::Scene::New("Untitled", project->GetConfig().startScenePath);

			// === Create default shaders
			std::filesystem::create_directories(project->GetConfig().assetsDirectoryPath / "Shaders");
			// Default 2D shader
			Eklipse::Path dstPath = "//Shaders/Default2D.eksh";
			Eklipse::CopyFileContent(dstPath, "Assets/Shaders/Default2D.eksh");
			project->GetAssetLibrary()->GetShader(dstPath);
			// Default 3D shader
			dstPath = "//Shaders/Default3D.eksh";
			Eklipse::CopyFileContent(dstPath, "Assets/Shaders/Default3D.eksh");
			project->GetAssetLibrary()->GetShader(dstPath);

			// === Save project
			std::filesystem::path projectFilePath = path.path() / (name + EK_PROJECT_FILE_EXTENSION);
			bool saved = Eklipse::Project::Save(project, projectFilePath);
			EK_ASSERT(saved, "Failed to save project!");

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
		Eklipse::Scene::Save(Eklipse::Application::Get().GetScene());
	}
	void EditorLayer::OnProjectUnload()
	{
		Eklipse::Renderer::WaitDeviceIdle();
		if (Eklipse::Project::GetActive())
			Eklipse::Project::GetActive()->UnloadAssets();
	}
	void EditorLayer::OnLoadResources()
	{
		m_assetLibrary->Load("Assets");
		m_filesPanel.LoadResources();
	}
	void EditorLayer::OnProjectLoaded()
	{
		ClearSelection();
		Eklipse::Project::GetActive()->LoadAssets();
		m_filesPanel.OnContextChanged();

		Eklipse::Application::Get().GetScene()->ApplyAllComponents();
	}
	void EditorLayer::SetSelection(DetailsSelectionInfo info)
	{
		m_selectionInfo = info;
	}
	void EditorLayer::ClearSelection()
	{
		GetSelection().type = SelectionType::None;
		GetSelection().entity.MarkNull();
	}
}