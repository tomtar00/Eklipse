#include "precompiled.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace Eklipse
{
    struct ScriptConfig
    {
        std::string projectName;
        std::string projectDir;

        std::string includeDir;
        std::string libDir;
    };
    static void replace_all(std::string& str, const std::string& from, const std::string& to)
    {
        size_t pos = str.find(from);
        do
        {
            str.replace(pos, from.length(), to);
            pos = str.find(from, pos + to.length());
        } while (pos != std::string::npos);
    }
    static bool GenerateLuaScript(const std::string& template_path, const std::string& output_path, const ScriptConfig& config)
    {
        std::ifstream template_file(template_path);
        if (!template_file.is_open())
        {
            EK_ERROR("Failed to open template file '{0}'", template_path);
            return false;
        }
        std::stringstream buffer;
        buffer << template_file.rdbuf();
        std::string template_content = buffer.str();

        replace_all(template_content, "__PRJ_NAME__", config.projectName);
        replace_all(template_content, "__PRJ_DIR__", config.projectDir);
        replace_all(template_content, "__INCLUDE_DIR__", config.includeDir);
        replace_all(template_content, "__LIB_DIR__", config.libDir);

        std::ofstream output_file(output_path);
        if (!output_file.is_open())
        {
            EK_ERROR("Failed to open output file '{0}'", output_path);
            return false;
        }
        output_file << template_content;

        return true;
    }

    Ref<Project> Project::s_activeProject = nullptr;

    Project::Project()
    {
        m_assetLibrary = CreateRef<AssetLibrary>();
    }
    void Project::LoadAssets()
    {
        m_assetLibrary->Load(m_config.assetsDirectoryPath);
    }
    void Project::UnloadAssets()
    {
        m_assetLibrary->Unload();
    }

    const std::filesystem::path& Project::GetProjectDirectory()
    {
        EK_ASSERT(s_activeProject != nullptr, "Project is null!");
        return s_activeProject->m_projectDirectory;
    }
    Ref<Project> Project::New()
    {
        s_activeProject = CreateRef<Project>();
        return s_activeProject;
    }
    void Project::SetupActive(const std::string& name, const Path& projectDirectory)
    {
        s_activeProject->m_projectDirectory = projectDirectory;

        // Create config
        std::string defaultSceneName = "Unititled";
        s_activeProject->GetConfig().name = name;
        s_activeProject->GetConfig().assetsDirectoryPath = projectDirectory / "Assets";
        s_activeProject->GetConfig().startScenePath = s_activeProject->GetConfig().assetsDirectoryPath / "Scenes" / (defaultSceneName + EK_SCENE_FILE_EXTENSION);
        s_activeProject->GetConfig().scriptsDirectoryPath = projectDirectory / "Scripts";
        s_activeProject->GetConfig().scriptsSourceDirectoryPath = s_activeProject->GetConfig().scriptsDirectoryPath / "Source";
        s_activeProject->GetConfig().buildDirectoryPath = projectDirectory / "Build";

        std::filesystem::create_directories(s_activeProject->GetConfig().assetsDirectoryPath / "Scenes");
        std::filesystem::create_directories(s_activeProject->GetConfig().assetsDirectoryPath / "Shaders");
        std::filesystem::create_directories(s_activeProject->GetConfig().scriptsSourceDirectoryPath);
        std::filesystem::create_directories(s_activeProject->GetConfig().buildDirectoryPath);

        // Default 2D shader
        Eklipse::Path dstPath = "//Shaders/Default2D.eksh";
        Eklipse::CopyFileContent(dstPath, "Assets/Shaders/Default2D.eksh");
        s_activeProject->GetAssetLibrary()->GetShader(dstPath);
        // Default 3D shader
        dstPath = "//Shaders/Default3D.eksh";
        Eklipse::CopyFileContent(dstPath, "Assets/Shaders/Default3D.eksh");
        s_activeProject->GetAssetLibrary()->GetShader(dstPath);

        // Generate premake5.lua
        auto premakeDir = s_activeProject->GetConfig().scriptsDirectoryPath / "Resources" / "Premake";
        std::filesystem::create_directories(premakeDir);

        auto currentPath = std::filesystem::current_path();
        ScriptConfig config{};
        config.projectName = name;
        config.projectDir = projectDirectory.full_string();
        config.includeDir = Eklipse::Path(currentPath / "Resources/Scripting/Include").full_string();
        config.libDir = Eklipse::Path(currentPath / "Resources/Scripting/Lib").full_string();

        auto premakeScriptPath = premakeDir / "premake5.lua";
        bool success = GenerateLuaScript("Resources/Scripting/Premake/premake5.lua", premakeScriptPath.string(), config);
        EK_ASSERT(success, "Failed to generate premake5.lua!");
        EK_CORE_INFO("Generated premake5.lua at path '{0}'", premakeScriptPath.string());

        // Run .lua script to generate project files
#ifdef EK_PLATFORM_WINDOWS
        std::string command = "cd " + (currentPath / "Resources/Scripting/Premake").string() + " && premake5.exe vs2022 --file=" + premakeScriptPath.string();
#elif defined(EK_PLATFORM_LINUX)
        std::string command = "cd " + (currentPath / "Resources/Scripting/Premake").string() + " && premake5 gmake2 --file=" + premakeScriptPath.string();
#elif defined(EK_PLATFORM_MACOS)
        std::string command = "cd " + (currentPath / "Resources/Scripting/Premake").string() + " && premake5 xcode4 --file=" + premakeScriptPath.string();
#endif
        EK_CORE_INFO("Running command: {0}", command);
        int res = system(command.c_str());
        EK_ASSERT(res == 0, "Failed to run premake5.lua!");

        s_activeProject->m_scriptManager.Load(s_activeProject);
	}
    Ref<Project> Project::Load(const std::filesystem::path& projectFilePath)
    {
        Ref<Project> project = CreateRef<Project>();

        ProjectSerializer serializer(project);
        if (serializer.Deserialize(projectFilePath))
        {
            project->m_projectDirectory = projectFilePath.parent_path();
            s_activeProject = project;

            s_activeProject->m_scriptManager.Load(s_activeProject);

            return s_activeProject;
        }

        EK_CORE_WARN("Failed to load project file '{0}'", projectFilePath.string());
        return nullptr;
    }
    bool Project::Save(Ref<Project> project, const std::filesystem::path& projectFilePath)
    {
        ProjectSerializer serializer(project);
        if (serializer.Serialize(projectFilePath))
        {
            project->m_projectDirectory = projectFilePath.parent_path();
            return true;
        }

        EK_CORE_WARN("Failed to save project file '{0}'", projectFilePath.string());
        return false;
    }
    bool Project::SaveActive()
    {
        return Save(
            s_activeProject, 
            s_activeProject->m_projectDirectory / (s_activeProject->m_config.name + EK_PROJECT_FILE_EXTENSION)
        );
    }
    bool Project::Exists(const std::filesystem::path& projectDirPath)
    {
        for (const auto& entry : std::filesystem::directory_iterator(projectDirPath))
        {
            if (entry.path().extension() == EK_PROJECT_FILE_EXTENSION)
            {
                EK_CORE_WARN("Project file already exists in directory '{0}'!", projectDirPath.string());
                return true;
            }
        }

        return false;
    }
}