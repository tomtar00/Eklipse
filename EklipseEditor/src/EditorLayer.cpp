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
    }

    // === Layer ===
    void EditorLayer::OnAttach()
    {
        m_editorCamera.m_farPlane = 1000.0f;
        m_editorCamera.m_nearPlane = 0.1f;
        m_editorCamera.m_fov = 45.0f;

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
            &m_debugPanel,
            &m_terminalPanel
        };

        m_editorScene = CreateRef<Scene>();
        m_entitiesPanel.SetContext(m_editorScene);
        SceneManager::SetActiveScene(m_editorScene);

        m_scriptManager = CreateRef<ScriptManager>(&m_settings.ScriptManagerSettings);

        Log::AddCoreSink(m_terminalPanel.GetTerminal().GetSink());
        Log::AddClientSink(m_terminalPanel.GetTerminal().GetSink());

        EklipseScriptAPI::ScriptingConfig config{};
        // Logging
        {
            config.loggerConfig.name = "SCRIPT";
            config.loggerConfig.pattern = "%^[%T] %n: %v%$";
            config.loggerConfig.sink = m_terminalPanel.GetTerminal().GetSink();
        }
        EklipseScriptAPI::Init(config);

        DeserializeSettings();
        EK_TRACE("Editor layer attached");
    }
    void EditorLayer::OnDetach()
    {
        SerializeSettings();
        m_editorScene.reset();
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
                float mouseXDelta = -Input::GetMouseDeltaX();
                float mouseYDelta = Input::GetMouseDeltaY();

                pitch -= mouseYDelta * deltaTime * 100.f;
                pitch = glm::clamp(pitch, -89.0f, 89.0f);
                yaw -= mouseXDelta * deltaTime * 100.f;
            }
            else if (Input::IsMouseButtonDown(Button2))
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
        Renderer::BeginRenderPass(m_viewportFramebuffer.get());

        bool isPlaying = SceneManager::GetActiveScene()->GetState() == SceneState::RUNNING;

        if (isPlaying)
            SceneManager::GetActiveScene()->OnSceneUpdate(deltaTime);

        if (isPlaying)
            Renderer::RenderScene(SceneManager::GetActiveScene());
        else
            Renderer::RenderScene(SceneManager::GetActiveScene(), m_editorCamera, m_editorCameraTransform);

        Renderer::EndRenderPass(m_viewportFramebuffer.get());

        Renderer::BeginRenderPass(g_defaultFramebuffer);
        GUI->Render();
        Renderer::EndRenderPass(g_defaultFramebuffer);
        // ===================================
    }
    void EditorLayer::OnGUI(float deltaTime)
    {
        static bool openNewProjectPopup = false;
        static bool openExportProjectPopup = false;

        if (ImGui::BeginMainMenuBar())
        {
            bool isPlaying = SceneManager::GetActiveScene()->GetState() == SceneState::RUNNING;
            bool isPaused = SceneManager::GetActiveScene()->GetState() == SceneState::PAUSED;

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItemEx("New Project", nullptr, "Ctrl+N", false, !isPlaying))
                {
                    openNewProjectPopup = true;
                }
                if (ImGui::MenuItemEx("Open Project", nullptr, "Ctrl+O", false, !isPlaying))
                {
                    OpenProject(); // TODO: save and load created projects list and show it in modal/window
                }
                if (ImGui::MenuItemEx("Save Project", nullptr, "Ctrl+S", false, !isPlaying))
                {
                    SaveProject();
                }
                if (ImGui::MenuItemEx("Save Project As", nullptr, "Ctrl+Shift+S", false, !isPlaying))
                {
                    SaveProjectAs();
                }
                if (ImGui::MenuItemEx("Save Scene", nullptr, nullptr, false, !isPlaying))
                {
                    SaveScene();
                }
                if (ImGui::MenuItemEx("Export", nullptr, nullptr, false, !isPlaying))
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
                if (ImGui::MenuItemEx("Play", nullptr, "Ctrl+P", false, !isPlaying))
                {
                    OnScenePlay();
                }
                if (ImGui::MenuItemEx("Pause", nullptr, "Ctrl+Shift+P", false, !isPlaying))
                {
                    OnScenePause();
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

        // === NEW PROJECT POPUP ===
        if (openNewProjectPopup)
        {
            ImGui::OpenPopup("Create New Project");
            openNewProjectPopup = false;
        }
        if (ImGui::BeginPopupModal("Create New Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
        {
            static String nameBuffer;
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
    
    // === API Events ===
    void EditorLayer::OnAPIHasInitialized(ApiType api)
    {
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

        GUI.reset();
        GUI = ImGuiLayer::Create(m_guiLayerCreateInfo);
        Application::Get().PushOverlay(GUI);
        GUI->Init();

        m_filesPanel.LoadResources();
    }
    void EditorLayer::OnShutdownAPI()
    {
        Application::Get().PopOverlay(GUI);
        ClearSelection();

        GUI->Shutdown();

        m_filesPanel.UnloadResources();

        //m_viewportFramebuffer->Dispose();
        //m_viewportFramebuffer.reset();
    }
    
    // === Project ===
    void EditorLayer::NewProject(const Path& dirPath, const String& name)
    {
        OnProjectUnload();

        auto project = Project::New();
        if (Project::Exists(dirPath))
        {
            EK_ERROR("Project already exists!");
            return;
        }
        if (!Project::SetupActive(name, dirPath))
        {
            EK_ERROR("Failed to setup project!");
            return;
        }

        m_scriptManager->RunPremake(Project::GetActive()->GetConfig().scriptPremakeDirectoryPath);
        m_scriptManager->Load();

        auto scene = Scene::New();
        SceneManager::SetActiveScene(scene);

        Path projectFilePath = dirPath / (name + EK_PROJECT_EXTENSION);
        Project::Save(project, projectFilePath);

        m_editorScene.reset();
        m_editorScene = scene;
        m_entitiesPanel.SetContext(m_editorScene);

        OnProjectLoaded();
    }
    void EditorLayer::OpenProject()
    {
        auto& result = FileUtilities::OpenFileDialog({ EK_PROJECT_EXTENSION });
        if (result.type == FileDialogResultType::SUCCESS)
        {
            OnProjectUnload();

            auto project = Project::Load(result.path);
            m_scriptManager->Load();

            auto scene = Scene::Load(project->GetConfig().startScenePath);
            SceneManager::SetActiveScene(scene);

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

        Path scenePath = AssetManager::GetMetadata(m_editorScene->Handle).FilePath;
        Scene::Save(m_editorScene, scenePath);
    }
    void EditorLayer::ExportProject(const ProjectExportSettings& exportSettings)
    {
        EK_ASSERT(Project::GetActive(), "Project is null!");

        SaveProject();
        if (Project::GetActive()->GetConfig().configuration != exportSettings.configuration)
        {
            m_scriptManager->CompileScripts(Project::GetActive()->GetConfig().scriptsSourceDirectoryPath, exportSettings.configuration);
        }
        if (!ProjectExporter::Export(Project::GetActive(), exportSettings))
        {
            EK_ERROR("Failed to export project!");
        }
    }
    
    // === Settings ===
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

        out << YAML::Key << "ScriptManager" << YAML::Value;
        {
            out << YAML::BeginMap;
            out << YAML::Key << "MsBuildPath" << YAML::Value << m_settings.ScriptManagerSettings.MsBuildPath.string();
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

        TryDeserailize<String>(preferencesNode, "Theme", &m_settings.theme);

        auto& scriptModuleNode = data["ScriptManager"];
        if (scriptModuleNode)
        {
            m_settings.ScriptManagerSettings.MsBuildPath = TryDeserailize<String>(scriptModuleNode, "MsBuildPath", "");
        }
    }
    
    // === Scene Events ===
    void EditorLayer::OnScenePlay()
    {
        if (SceneManager::GetActiveScene()->GetState() == SceneState::RUNNING)
            return;

        m_canControlEditorCamera = false;

        auto sceneCopy = Scene::Copy(m_editorScene);
        SceneManager::SetActiveScene(sceneCopy);
        SceneManager::GetActiveScene()->OnSceneStart();

        m_entitiesPanel.SetContext(sceneCopy);
        ClearSelection();
    }
    void EditorLayer::OnSceneStop()
    {
        if (SceneManager::GetActiveScene()->GetState() == SceneState::NONE)
            return;

        m_canControlEditorCamera = true;

        SceneManager::GetActiveScene()->OnSceneStop();
        SceneManager::SetActiveScene(m_editorScene);

        m_entitiesPanel.SetContext(m_editorScene);
        ClearSelection();

        // Recompile scripts if they were changed while playing
        if (Project::GetActive())
        {
            if (m_scriptManager->GetScriptsState() == ScriptsState::NEEDS_RECOMPILATION)
            {
                m_scriptManager->RecompileAll();
            }
        }
    }
    void EditorLayer::OnScenePause()
    {
        if (SceneManager::GetActiveScene()->GetState() == SceneState::PAUSED)
            return;

        SceneManager::GetActiveScene()->OnScenePause();
        m_canControlEditorCamera = false;
    }
    void EditorLayer::OnSceneResume()
    {
        if (SceneManager::GetActiveScene()->GetState() == SceneState::RUNNING)
            return;

        SceneManager::GetActiveScene()->OnSceneResume();
        m_canControlEditorCamera = false;
    }
    
    // === Project Events ===
    void EditorLayer::OnProjectUnload()
    {
        // unload assets
    }
    void EditorLayer::OnProjectLoaded()
    {
        EK_ASSERT(Project::GetActive(), "Project is null!");

        ClearSelection();
        m_editorAssetLibrary = CreateRef<EditorAssetLibrary>(
            Project::GetActive()->GetConfig().assetsDirectoryPath
        );
        m_filesPanel.OnContextChanged();
        m_editorScene->ApplyAllComponents();
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
    bool EditorLayer::IsPlaying() const
    {
        return SceneManager::GetActiveScene()->GetState() == SceneState::RUNNING;
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

            ImGui::SeparatorText("AssetRegistry");
            if (ImGui::BeginTable("Assets##Table", 2))
            {
                ImGui::TableSetupColumn("Handle", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (auto& [handle, metadata] : m_editorAssetLibrary->GetAssetRegistry())
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", handle);

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
                ImGui::TextUnformatted("Scripts path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptsDirectoryPath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts source path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptsSourceDirectoryPath.string().c_str());

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted("Scripts build path");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(config.scriptBuildDirectoryPath.string().c_str());

                ImGui::EndTable();
            }

            ImGui::SeparatorText("Scripts");
            for (auto&& [name, config] : ScriptLinker::Get().GetScriptClasses())
            {
                if (ImGui::CollapsingHeader(name.c_str()))
                {
                    ImGui::Spacing();
                    ImGui::Indent();
                    ImGui::Text("Create function exists: %s", (config.create != nullptr ? "true" : "false"));
                    for (auto&& [name, member] : config.members)
                    {
                        ImGui::Text("Name: %s   Type: %s   Offset: %d", name.c_str(), member.type.c_str(), member.offset);
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
}