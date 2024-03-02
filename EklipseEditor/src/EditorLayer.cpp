#include "EditorLayer.h"
#include <Eklipse/Scene/Components.h>
#include <Eklipse/Project/Project.h>

#include <glm/gtc/quaternion.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <filesystem>

#define EK_EDITOR_CONFIG_FILE "config.yaml"

namespace Eklipse
{
    EditorLayer::EditorLayer() : m_guiEnabled(true), m_isWindowMaximized(false), m_canControlEditorCamera(true)
    {
        EK_ASSERT(s_instance == nullptr, "Editor layer already exists!");
        s_instance = this;

        ImGuiLayerConfig guiConfig{};
        guiConfig.menuBarEnabled = true;
        guiConfig.dockingEnabled = true;
        guiConfig.dockLayouts =
        {
            { "Entities",	ImGuiDir_Left,	Dir_Opposite,	0.7f },
            { "Settings",	ImGuiDir_Down,	Dir_Same,		1.0f },
            { "Stats",		ImGuiDir_Down,	Dir_Same,		1.0f },
            { "Details",	ImGuiDir_Right,	Dir_Opposite,	0.7f },
            { "Files",		ImGuiDir_Down,	Dir_Opposite,	0.8f },
            { "Profiler",	ImGuiDir_Down,	Dir_Stack,	    1.0f },
            { "Terminal",	ImGuiDir_Right,	Dir_Same,		0.5f },
            { "View",		ImGuiDir_Up,	Dir_Rest,		1.0f },
            { "Debug",		ImGuiDir_Down,	Dir_Stack,		1.0f }
        };
        guiConfig.panels =
        {
            &m_entitiesPanel,
            &m_settingsPanel,
            &m_statsPanel,
            &m_detailsPanel,
            &m_profilerPanel,
            &m_viewPanel,
            &m_filesPanel,
            &m_debugPanel,
            &m_terminalPanel
        };
        GUI = CreateRef<ImGuiLayer>(guiConfig);
    }

    // === Layer ===
    void EditorLayer::OnAttach()
    {
        m_editorCamera.m_farPlane = 1000.0f;
        m_editorCamera.m_nearPlane = 0.1f;
        m_editorCamera.m_fov = 45.0f;

        m_isWindowMaximized = false;

        m_editorState = EditorState::EDIT;
        m_editorScene = CreateRef<Scene>();
        m_entitiesPanel.SetContext(m_editorScene.get());
        SceneManager::SetActiveScene(m_editorScene);

        m_scriptManager = CreateRef<ScriptManager>(&m_settings.ScriptManagerSettings);

        auto terminalSink = m_terminalPanel.GetTerminal().GetSink();
        
        Log::AddCoreSink(terminalSink);
        Log::AddClientSink(terminalSink);
        
        EklipseScriptAPI::ScriptingConfig scriptConfig{};
        // Logging
        {
            scriptConfig.loggerConfig.name = "SCRIPT";
            scriptConfig.loggerConfig.pattern = "%^[%T] %n: %v%$";
            scriptConfig.loggerConfig.sink = terminalSink;
        }
        EklipseScriptAPI::Init(scriptConfig);

        AddDebugDrawInfo();
        AddTeminalCommands();

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

            if (Input::IsKeyDown(F))
            {
                if (SelectionInfo.type == SelectionType::ENTITY)
                    targetPosition = SelectionInfo.entity.GetComponent<TransformComponent>().transform.position;
            }
            else if (Input::IsMouseButtonDown(Button1))
            {
                glm::vec2 mouseDelta = Input::GetMouseDelta();
                float mouseXDelta = mouseDelta.x;
                float mouseYDelta = -mouseDelta.y;

                pitch -= mouseYDelta * deltaTime * m_settings.editorCameraRotationSpeed;
                pitch = glm::clamp(pitch, -89.0f, 89.0f);
                yaw -= mouseXDelta * deltaTime * m_settings.editorCameraRotationSpeed;
            }
            else if (Input::IsMouseButtonDown(Button2))
            {
                glm::vec2 mouseDelta = Input::GetMouseDelta();
                float mouseXDelta = -mouseDelta.x;
                float mouseYDelta = mouseDelta.y;

                glm::vec3 cameraDir = glm::normalize(targetPosition - m_editorCameraTransform.position);
                glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDir, cameraUp));
                targetPosition += (mouseXDelta * cameraRight + mouseYDelta * cameraUp) * deltaTime * m_settings.editorCameraDragSpeed;
            }

            glm::vec3 cameraPosition{};
            cameraPosition.x = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            cameraPosition.y = distance * sin(glm::radians(pitch));
            cameraPosition.z = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
            cameraPosition += targetPosition;

            m_editorCameraTransform.position = cameraPosition;
            glm::vec3 cameraDir = glm::normalize(targetPosition - m_editorCameraTransform.position);
            m_editorCameraTransform.rotation = glm::degrees(glm::eulerAngles(glm::quatLookAt(cameraDir, cameraUp)));
        }
        // ===================================

        // == Input ==========================
        if (m_editorState == EditorState::PLAY)
        {
            if (Input::IsKeyDown(Escape))
            {
                Application::Get().GetWindow()->SetCursorMode(CursorMode::Normal);
            }
        }

        // == DRAW ===========================
        Renderer::BeginRenderPass(m_viewportFramebuffer.get());

        if (m_editorState == EditorState::PLAY)
            SceneManager::GetActiveScene()->OnSceneUpdate(deltaTime);

        if (m_editorState == EditorState::EDIT)
            Renderer::RenderScene(SceneManager::GetActiveScene(), m_editorCamera, m_editorCameraTransform);
        else
            Renderer::RenderScene(SceneManager::GetActiveScene());

        Renderer::EndRenderPass(m_viewportFramebuffer.get());

        // ===================================
    }
    void EditorLayer::OnGUI(float deltaTime)
    {
        static bool openNewProjectPopup = false;
        static bool openExportProjectPopup = false;
        
        if (m_pauseRequested)
        {
            m_pauseRequested = false;
            OnScenePause();
        }

        if (!Project::GetActive())
        {
            static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            if (!openNewProjectPopup)
                ImGui::SetNextWindowFocus();
            if (ImGui::Begin("Select project", nullptr, flags))
            {
                if (ImGui::Button("New Project"))
                {
                    openNewProjectPopup = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Open Project"))
                {
                    OpenProject();
                }

                ImGui::Separator();

                ImGui::Text("Recent Projects");
                float width = ImGui::GetContentRegionAvail().x;
                ProjectHandle toRemove = 0;
                for (auto& [handle, metadata] : m_settings.projectRegistry)
                {
                    ImGui::PushID(handle);
                    if (ImGui::Button(metadata.path.stem().string().c_str(), { width-80, 30 }))
                    {
                        OpenProject(handle);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Remove", { 80, 30 }))
                    {
                        toRemove = handle;
                    }
                    ImGui::PopID();
                }
                if (toRemove != 0)
                {
                    m_settings.projectRegistry.erase(toRemove);
                }
            }
            ImGui::End();

            if (openNewProjectPopup)
            {
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowFocus();

                if (ImGui::Begin("Create New Project", nullptr, flags))
                {
                    static String projectName;
                    static Path projectsLocation = m_settings.projectsPath;
                    ImGui::DrawProperty("prjName", "Project Name", [&]() {
                        ImGui::InputText("##Project Name", &projectName);
                    });

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    if (ImGui::CollapsingHeader("Advanced"))
                    {
                        ImGui::InputDirPath("prjlocation", "Projects Location", projectsLocation);
                    }

                    ImGui::Spacing();

                    if (ImGui::Button("Create") && NewProject(projectsLocation / projectName, projectName))
                    {
                        openNewProjectPopup = false;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel"))
                    {
                        openNewProjectPopup = false;
                    }

                }
                ImGui::End();
            }
        }
        else 
        {
            if (ImGui::BeginMainMenuBar())
            {
                bool isPlaying = m_editorState == EditorState::PLAY;
                bool isPaused = m_editorState == EditorState::PAUSE;
                bool isEditing = m_editorState == EditorState::EDIT;

                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItemEx("New Project", nullptr, "Ctrl+N", false, isEditing))
                    {
                        openNewProjectPopup = true;
                    }
                    if (ImGui::MenuItemEx("Open Project", nullptr, "Ctrl+O", false, isEditing))
                    {
                        OpenProject(); // TODO: save and load created projects list and show it in modal/window
                    }
                    if (ImGui::MenuItemEx("Save Project", nullptr, "Ctrl+S", false, isEditing))
                    {
                        SaveProject();
                    }
                    if (ImGui::MenuItemEx("Save Scene", nullptr, "Ctrl+Shift+S", false, isEditing))
                    {
                        SaveScene();
                    }
                    if (ImGui::MenuItemEx("Export", nullptr, nullptr, false, isEditing))
                    {
                        openExportProjectPopup = true;
                    }
                    if (ImGui::MenuItemEx("Exit", nullptr, nullptr))
                    {
                        Application::Get().Close();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Scene"))
                {
                    if (ImGui::MenuItemEx("Play", nullptr, "Ctrl+P", false, isEditing))
                    {
                        OnScenePlay();
                    }
                    if (ImGui::MenuItemEx("Pause", nullptr, "Ctrl+Shift+P", false, isPlaying))
                    {
                        RequestPause();
                    }
                    if (ImGui::MenuItemEx("Resume", nullptr, "Ctrl+Shift+R", false, isPaused))
                    {
                        OnSceneResume();
                    }
                    if (ImGui::MenuItemEx("Stop", nullptr, "Ctrl+Q", false, isPaused || isPlaying))
                    {
                        OnSceneStop();
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
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
                ImGui::InputDirPath("exportpath", "Export path", exportSettings.path);

#if EK_DEBUG
                static int configurationIndex = 0;
                ImGui::DrawProperty("isDevBuild", "Build Type", [&]() {
                    if (ImGui::Combo("##Configuration", &configurationIndex, "Debug\0Developement\0Release\0"))
                    {
                        if (configurationIndex == 0)
                            exportSettings.buildType = ProjectExportBuildType::DEBUG;
                        else if (configurationIndex == 1)
                            exportSettings.buildType = ProjectExportBuildType::Developement;
                        else if(configurationIndex == 2)
                            exportSettings.buildType = ProjectExportBuildType::Release;
                    }
                });
#else
                ImGui::DrawProperty("isDevBuild", "Development Build", [&]() {
                    static bool isDevBuild = false;
                    ImGui::Checkbox("##isDevBuild", &isDevBuild);
                    exportSettings.buildType = isDevBuild ? ProjectExportBuildType::Developement : ProjectExportBuildType::Release;
                });
#endif
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

            // === NEW PROJECT POPUP ===
            if (openNewProjectPopup)
            {
                ImGui::OpenPopup("Create New Project");
                openNewProjectPopup = false;
                ImGui::SetNextWindowSize({ 500, 150 });
            }
            if (ImGui::BeginPopupModal("Create New Project", nullptr, ImGuiWindowFlags_NoResize))
            {
                static String projectName;
                static Path projectsLocation = m_settings.projectsPath;
                ImGui::Text("Project Name");
                ImGui::SameLine();
                ImGui::InputText("##Project Name", &projectName);
                if (ImGui::CollapsingHeader("Advanced"))
                {
                    ImGui::InputDirPath("prjlocation", "Projects Location", projectsLocation);
                }

                if (ImGui::Button("Create") && NewProject(projectsLocation / projectName, projectName))
                {
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
    }
    
    // === API Events ===
    void EditorLayer::OnAPIHasInitialized(GraphicsAPI::Type api)
    {
        EK_PROFILE();

        // Create off-screen framebuffer (for scene view)
        {
            FramebufferInfo fbInfo{};
            fbInfo.width					= GetViewPanel().GetViewportSize().x > 0 ? GetViewPanel().GetViewportSize().x : 512;
            fbInfo.height					= GetViewPanel().GetViewportSize().y > 0 ? GetViewPanel().GetViewportSize().y : 512;
            fbInfo.numSamples				= Renderer::GetSettings().GetMsaaSamples();
            fbInfo.colorAttachmentInfos		= { { ImageFormat::RGBA8 } };
            fbInfo.depthAttachmentInfo		= { ImageFormat::D24S8 };

            m_viewportFramebuffer = Framebuffer::Create(fbInfo);
        }

        if (m_isWindowMaximized)
            Application::Get().GetWindow()->Maximize();

        SetTheme(m_settings.theme);

        m_filesPanel.LoadResources();
        if (m_editorAssetLibrary)
        {
            m_editorAssetLibrary->ReloadAssets();
        }
        if (m_editorScene)
        {
            m_editorScene->ApplyAllComponents();
        }
    }
    void EditorLayer::OnShutdownAPI(bool quit)
    {
        EK_PROFILE();

        ClearSelection();

        m_filesPanel.UnloadResources();

        m_isWindowMaximized = Application::Get().GetWindow()->IsMaximized();

        if (Project::GetActive())
        {
            m_editorAssetLibrary->UnloadAssets();
            
        }
        if (quit)
        {
            m_editorScene.reset();
            m_editorScene = nullptr;
        }
    }
    
    // === Project ===
    bool EditorLayer::NewProject(const Path& dirPath, const String& name)
    {
        EK_PROFILE();

        if (dirPath.empty()) {
            EK_ERROR("Project directory path is empty!"); 
            return false;
        }
        if (FileUtilities::IsPathValid(dirPath)) {
            EK_ERROR("Project directory path is invalid!"); 
            return false;
        }
        if (name.empty()) {
            EK_ERROR("Project name is empty!"); 
            return false;
        }
        if (Project::Exists(dirPath)) {
            EK_ERROR("Project already exists in directory '{}'!", dirPath.string());
            return false;
        }

        EK_INFO("Creating new project '{}' at '{}'", name, dirPath.string());

        OnProjectUnload();

        auto project = Project::New();
        if (!Project::SetupActive(name, dirPath))
        {
            EK_ERROR("Failed to setup project!");
            return false;
        }
        auto& config = Project::GetActive()->GetConfig();

        // init asset library
        m_editorAssetLibrary = CreateRef<EditorAssetLibrary>(config.assetsDirectoryPath);

        // setup script manager
        m_scriptManager->RunPremake(Project::GetActive()->GetConfig().scriptPremakeDirectoryPath);
        m_scriptManager->Load();

        // import start scene
        AssetHandle sceneHandle = m_editorAssetLibrary->ImportAsset(config.startScenePath);
        config.startSceneHandle = sceneHandle;

        // import default assets
        m_cubeHandle = m_editorAssetLibrary->ImportAsset("Assets/Meshes/cube.obj");
        m_sphereHandle = m_editorAssetLibrary->ImportAsset("Assets/Meshes/sphere.obj");
        m_shader3dHandle = m_editorAssetLibrary->ImportAsset("Assets/Shaders/3D.glsl");
        m_shader2dHandle = m_editorAssetLibrary->ImportAsset("Assets/Shaders/2D.glsl");
        m_material3dHandle = m_editorAssetLibrary->ImportDefaultMaterial(config.assetsDirectoryPath / "Materials/3D.ekmt", m_shader3dHandle);
        m_material2dHandle = m_editorAssetLibrary->ImportDefaultMaterial(config.assetsDirectoryPath / "Materials/2D.ekmt", m_shader2dHandle);

        // get start scene
        m_editorScene = AssetManager::GetAsset<Scene>(sceneHandle);
        m_entitiesPanel.SetContext(m_editorScene.get());
        SceneManager::SetActiveScene(m_editorScene);

        // save project
        Project::SaveActive();
        m_editorAssetLibrary->StartFileWatcher();

        // add to project registry
        ProjectHandle projectHandle;
        ProjectMetadata metadata;
        metadata.path = dirPath / (name + String(EK_PROJECT_EXTENSION));
        metadata.lastAccessTime = Timer::Now().time_since_epoch().count();
        m_settings.projectRegistry[projectHandle] = metadata;

        OnProjectLoaded();

        EK_INFO("Project created successfully!");
        return true;
    }
    void EditorLayer::OpenProject(const Path& path)
    {
        EK_PROFILE();

        EK_INFO("Opening project at path '{}'", path.string());
        OnProjectUnload();

        auto project = Project::Load(path);
        auto& config = project->GetConfig();

        m_editorAssetLibrary = CreateRef<EditorAssetLibrary>(config.assetsDirectoryPath);
        if (!m_editorAssetLibrary->DeserializeAssetRegistry())
        {
            EK_ERROR("Failed to deserialize asset registry!");
        }

        m_scriptManager->Load();

        // get default assets
        m_cubeHandle = m_editorAssetLibrary->GetHandleFromAssetPath("Assets/Meshes/cube.obj");
        m_sphereHandle = m_editorAssetLibrary->GetHandleFromAssetPath("Assets/Meshes/sphere.obj");
        m_shader3dHandle = m_editorAssetLibrary->GetHandleFromAssetPath("Assets/Shaders/3D.glsl");
        m_shader2dHandle = m_editorAssetLibrary->GetHandleFromAssetPath("Assets/Shaders/2D.glsl");
        m_material3dHandle = m_editorAssetLibrary->GetHandleFromAssetPath(config.assetsDirectoryPath / "Materials/3D.ekmt");
        m_material2dHandle = m_editorAssetLibrary->GetHandleFromAssetPath(config.assetsDirectoryPath / "Materials/2D.ekmt");

        m_editorScene = AssetManager::GetAsset<Scene>(config.startSceneHandle);
        SceneManager::SetActiveScene(m_editorScene);
        m_entitiesPanel.SetContext(m_editorScene.get());

        m_editorAssetLibrary->StartFileWatcher();

        OnProjectLoaded();
    }
    void EditorLayer::OpenProject(ProjectHandle handle)
    {
        EK_PROFILE();

        Path projectPath = m_settings.projectRegistry[handle].path;
        OpenProject(projectPath);
    }
    void EditorLayer::OpenProject()
    {
        EK_PROFILE();

        auto& result = FileUtilities::OpenFileDialog(Vec<String>{ EK_PROJECT_EXTENSION });
        if (result.type == FileDialogResultType::SUCCESS)
        {
            // search if this project is already in registry
            bool found = false;
            for (auto& [handle, metadata] : m_settings.projectRegistry)
            {
                if (metadata.path == result.path)
                {
                    metadata.lastAccessTime = Timer::Now().time_since_epoch().count();
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                ProjectHandle projectHandle;
                ProjectMetadata metadata;
                metadata.path = result.path;
                metadata.lastAccessTime = Timer::Now().time_since_epoch().count();
                m_settings.projectRegistry[projectHandle] = metadata;
            }

            OpenProject(result.path);
        }
    }
    void EditorLayer::SaveProject()
    {
        EK_PROFILE();

        if (!Project::GetActive()) return;

        SaveScene();
        Project::SaveActive();

        // TODO: needs refactor (materials do not save their constants automatically)
        for (auto& [handle, metadata] : m_editorAssetLibrary->GetAssetRegistry())
        {
            if (metadata.Type == AssetType::Material)
            {
                auto& metadata = m_editorAssetLibrary->GetMetadata(handle);
                AssetManager::GetAsset<Material>(handle)->ApplyChanges(metadata.FilePath);
            }
        }
    }
    void EditorLayer::SaveScene()
    {
        EK_PROFILE();

        if (!Project::GetActive()) return;

        auto& filePath = m_editorAssetLibrary->GetMetadata(m_editorScene->Handle).FilePath;
        Scene::Save(m_editorScene, filePath);
    }
    void EditorLayer::ExportProject(const ProjectExportSettings& exportSettings)
    {
        EK_PROFILE();

        EK_ASSERT(Project::GetActive(), "Project is null!");

        SaveProject();
        if (EK_CURRENT_CONFIG != exportSettings.buildType && ScriptLinker::Get().HasAnyScriptClasses())
        {
            m_scriptManager->CompileScripts(Project::GetActive()->GetConfig().scriptsSourceDirectoryPath, exportSettings.buildType);
        }
        if (!ProjectExporter::Export(m_editorAssetLibrary, Project::GetActive(), exportSettings))
        {
            EK_ERROR("Failed to export project!");
        }
    }
    
    // === Settings ===
    bool EditorLayer::SerializeSettings() const
    {
        EK_PROFILE();

        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Preferences" << YAML::Value;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "Theme" << YAML::Value << ThemeToString(m_settings.theme);
            out << YAML::Key << "ProjectsPath" << YAML::Value << m_settings.projectsPath;
            out << YAML::Key << "ProfilerEnabled" << YAML::Value << Profiler::Enabled;
            out << YAML::Key << "EditorCameraRotationSpeed" << YAML::Value << m_settings.editorCameraRotationSpeed;
            out << YAML::Key << "EditorCameraDragSpeed" << YAML::Value << m_settings.editorCameraDragSpeed;
            out << YAML::EndMap;
        }

        out << YAML::Key << "ScriptManager" << YAML::Value;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "MsBuildPath" << YAML::Value << m_settings.ScriptManagerSettings.MsBuildPath;
            out << YAML::EndMap;
        }

        out << YAML::Key << "ProjectRegistry" << YAML::Value;
        {
            out << YAML::BeginSeq;
            for (auto& [handle, metadata] : m_settings.projectRegistry)
            {
                out << YAML::BeginMap;
                out << YAML::Key << "Handle" << YAML::Value << handle;
                out << YAML::Key << "Path" << YAML::Value << metadata.path;
                out << YAML::Key << "LastAccessTime" << YAML::Value << metadata.lastAccessTime;
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
        }

        out << YAML::EndMap;

        std::ofstream fout(EK_EDITOR_CONFIG_FILE);
        fout << out.c_str();

        return true;
    }
    bool EditorLayer::DeserializeSettings()
    {
        EK_PROFILE();

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
        if (preferencesNode)
        {
            m_settings.theme = StringToTheme(TryDeserailize<String>(preferencesNode, "Theme", "Unknown"));
            TryDeserailize<Path>(preferencesNode, "ProjectsPath", &m_settings.projectsPath);
            TryDeserailize<bool>(preferencesNode, "ProfilerEnabled", &Profiler::Enabled);
            TryDeserailize<float>(preferencesNode, "EditorCameraRotationSpeed", &m_settings.editorCameraRotationSpeed);
            TryDeserailize<float>(preferencesNode, "EditorCameraDragSpeed", &m_settings.editorCameraDragSpeed);
        }
        else
        {
            EK_WARN("Failed to deserialize 'Preferences' node!");
        }

        auto& scriptModuleNode = data["ScriptManager"];
        if (scriptModuleNode)
        {
            TryDeserailize<Path>(scriptModuleNode, "MsBuildPath", &m_settings.ScriptManagerSettings.MsBuildPath);
        }
        else
        {
            EK_WARN("Failed to deserialize 'ScriptManager' node!");
        }

        auto& projectRegistryNode = data["ProjectRegistry"];
        if (projectRegistryNode)
        {
            for (auto& projectNode : projectRegistryNode)
            {
                ProjectHandle handle = projectNode["Handle"].as<ProjectHandle>();
                Path path = projectNode["Path"].as<Path>();
                uint64_t lastAccessTime = projectNode["LastAccessTime"].as<uint64_t>();

                ProjectMetadata metadata;
                metadata.path = path;
                metadata.lastAccessTime = lastAccessTime;
                m_settings.projectRegistry[handle] = metadata;
            }
        }
        else
        {
            EK_WARN("Failed to deserialize 'ProjectRegistry' node!");
        }
    }
    
    // === Scene Events ===
    void EditorLayer::OnScenePlay()
    {
        EK_PROFILE();

        if (m_editorState == EditorState::PLAY)
            return;

        m_editorState = EditorState::PLAY;
        m_canControlEditorCamera = false;

        auto sceneCopy = Scene::Copy(m_editorScene.get());
        SceneManager::SetActiveScene(sceneCopy);
        SceneManager::GetActiveScene()->OnSceneStart();

        m_entitiesPanel.SetContext(sceneCopy.get());
        ClearSelection();

        m_profilerPanel.OnPlay();
    }
    void EditorLayer::OnSceneStop()
    {
        EK_PROFILE();

        if (m_editorState == EditorState::EDIT)
            return;

        m_editorState = EditorState::EDIT;

        m_canControlEditorCamera = true;

        SceneManager::GetActiveScene()->OnSceneStop();
        SceneManager::SetActiveScene(m_editorScene);

        m_entitiesPanel.SetContext(m_editorScene.get());
        ClearSelection();

        // Recompile scripts if they were changed while playing
        if (Project::GetActive())
        {
            if (m_scriptManager->GetScriptsState() == ScriptsState::NEEDS_RECOMPILATION)
            {
                m_scriptManager->RecompileAll();
            }
        }

        Application::Get().GetWindow()->SetCursorMode(CursorMode::Normal);

        m_profilerPanel.OnStop();
    }
    void EditorLayer::OnScenePause()
    {
        EK_PROFILE();

        if (m_editorState != EditorState::PLAY)
            return;

        m_editorState = EditorState::PAUSE;

        SceneManager::GetActiveScene()->OnScenePause();
        m_canControlEditorCamera = false;

        Application::Get().GetWindow()->SetCursorMode(CursorMode::Normal);

        m_profilerPanel.OnPause();
    }
    void EditorLayer::OnSceneResume()
    {
        EK_PROFILE();

        if (m_editorState != EditorState::PAUSE)
            return;

        m_editorState = EditorState::PLAY;

        SceneManager::GetActiveScene()->OnSceneResume();
        m_canControlEditorCamera = false;

        m_profilerPanel.OnResume();
    }
    void EditorLayer::RequestPause()
    {
        m_pauseRequested = true;
    }
    
    // === Project Events ===
    void EditorLayer::OnProjectUnload()
    {
        EK_PROFILE();

        if (m_editorAssetLibrary)
        {
            m_editorAssetLibrary->UnloadAssets();
            m_editorAssetLibrary.reset();
        }
    }
    void EditorLayer::OnProjectLoaded()
    {
        EK_PROFILE();

        EK_ASSERT(Project::GetActive(), "Project is null!");

        ClearSelection();
        m_filesPanel.OnContextChanged();
        m_editorScene->ApplyAllComponents();

        Application::Get().GetWindow()->Maximize();
        m_isWindowMaximized = true;
    }
    
    // === Getters ===
    EditorLayer& EditorLayer::Get()
    {
        return *s_instance;
    }
    EditorSettings& EditorLayer::GetSettings()
    {
        return m_settings;
    }
    Framebuffer* EditorLayer::GetViewportFramebuffer() const
    {
        return m_viewportFramebuffer.get();
    }
    DetailsPanel& EditorLayer::GetDetailsPanel()
    {
        return m_detailsPanel;
    }
    const EntitiesPanel& EditorLayer::GetEntitiesPanel()
    {
        return m_entitiesPanel;
    }
    const ViewPanel& EditorLayer::GetViewPanel()
    {
        return m_viewPanel;
    }
    const Camera& EditorLayer::GetEditorCamera()
    {
        return m_editorCamera;
    }
    const EditorState EditorLayer::GetEditorState() const
    {
        return m_editorState;
    }
    const Ref<EditorAssetLibrary> EditorLayer::GetAssetLibrary() const
    {
        return m_editorAssetLibrary;
    }

    // === Setters ===	
    void EditorLayer::SetCanControlEditorCamera(bool canControl)
    {
        m_canControlEditorCamera = canControl;
    }

    // === GUI ===
    void EditorLayer::AddDebugDrawInfo()
    {
        m_debugPanel.AppendDrawRequest([&]() {

            if (!Project::GetActive()) return;

            ImGui::SeparatorText("Asset Registry");
            if (ImGui::BeginTable("Assets##Table", 3))
            {
                ImGui::TableSetupColumn("Handle", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (auto& [handle, metadata] : m_editorAssetLibrary->GetAssetRegistry())
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%llu", handle);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(Asset::TypeToString(metadata.Type).c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(metadata.FilePath.string().c_str());
                }

                ImGui::EndTable();
            }

            ImGui::SeparatorText("Project");
            if (ImGui::BeginTable("Config##Table", 2))
            {
                auto& config = Project::GetActive()->GetConfig();

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Name");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.name.c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Assets path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.assetsDirectoryPath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Start scene path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.startScenePath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Start scene handle");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%llu", config.startSceneHandle);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptsDirectoryPath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts resources path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptResourcesDirectoryPath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts generated path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptGeneratedDirectoryPath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts premake path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptPremakeDirectoryPath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts build path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptBuildDirectoryPath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts source path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptsSourceDirectoryPath.string().c_str());

                

                ImGui::EndTable();
            }

            ImGui::SeparatorText("Scripts");
            for (auto&& [name, classInfo] : ScriptLinker::Get().GetScriptClasses())
            {
                if (ImGui::CollapsingHeader(name.c_str()))
                {
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::Text("Create function exists: %s", (classInfo.create != nullptr ? "true" : "false"));
                    for (auto& [memberName, memberRef] : classInfo.reflection.members)
                    {
                        ImGui::Text("Name: %s   Type: %s   Offset: %d", name.c_str(), memberRef.memberType.c_str(), memberRef.memberOffset);
                    }
                    ImGui::Unindent();
                }
            }
            if (ImGui::BeginTable("Library##Table", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Library linked");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(ScriptLinker::Get().IsLibraryLinked() ? "true" : "false");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts state");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(m_scriptManager->GetState().c_str());

                ImGui::EndTable();
            }

            if (ImGui::Button("Recompile all"))
            {
                m_scriptManager->RecompileAll();
            }

        });
    }

    // === Selection ===
    void EditorLayer::ClearSelection()
    {
        SelectionInfo.type = SelectionType::NONE;
        SelectionInfo.entity.MarkNull();
    }
    
    void EditorLayer::AddTeminalCommands()
    {
        auto& terminal = m_terminalPanel.GetTerminal();

        terminal.AddCommand({
            "theme", "Set editor theme",
            "theme -style=<dark|darkgrey|darkblue|light>", {"style"},
            [&](const ParsedCommand& cmd)
            {
                if (cmd.Args.at("style") == "dark")
                {
                    SetTheme(Theme::Dark);
                    m_settings.theme = Theme::Dark;
                }
                else if (cmd.Args.at("style") == "darkgrey")
                {
                    SetTheme(Theme::DarkGrey);
                    m_settings.theme = Theme::DarkGrey;
                }
                else if (cmd.Args.at("style") == "darkblue")
                {
                    SetTheme(Theme::DarkBlue);
                    m_settings.theme = Theme::DarkBlue;
                }
                else if (cmd.Args.at("style") == "light")
                {
                    SetTheme(Theme::Light);
                    m_settings.theme = Theme::Light;
                }
                else
                {
                    EK_INFO("Invalid theme name!");
                }
            }
        });
    }

    // === Default Assets ===
    AssetHandle EditorLayer::GetDefaultCubeHandle() const
    {
        return m_cubeHandle;
    }
    AssetHandle EditorLayer::GetDefaultSphereHandle() const
    {
        return m_sphereHandle;
    }
    AssetHandle EditorLayer::GetDefault3DShaderHandle() const
    {
        return m_shader3dHandle;
    }
    AssetHandle EditorLayer::GetDefault2DShaderHandle() const
    {
        return m_shader2dHandle;
    }
    AssetHandle EditorLayer::GetDefault3DMaterialHandle() const
    {
        return m_material3dHandle;
    }
    AssetHandle EditorLayer::GetDefault2DMaterialHandle() const
    {
        return m_material2dHandle;
    }
}