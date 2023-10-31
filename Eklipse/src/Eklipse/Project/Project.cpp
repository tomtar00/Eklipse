#include "precompiled.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace Eklipse
{
    Ref<Project> Project::s_activeProject = nullptr;

    const std::filesystem::path& Project::GetProjectDirectory()
    {
        EK_ASSERT(s_activeProject != nullptr, "Project is null!");
        return s_activeProject->m_projectDirectory;
    }
    Ref<Project> Project::New(const std::filesystem::path& targetDirPath, const std::string& projectName)
    {
        std::filesystem::path projectFilePath = targetDirPath / (projectName + EK_PROJECT_FILE_EXTENSION);

        for (const auto& entry : std::filesystem::directory_iterator(targetDirPath)) 
        {
			if (entry.path().extension() == EK_PROJECT_FILE_EXTENSION)
            {
				EK_CORE_WARN("Project file '{0}' already exists!", projectFilePath.string());
				return nullptr;
			}
		}

        s_activeProject = CreateRef<Project>();
        s_activeProject->m_projectDirectory = targetDirPath;

        s_activeProject->m_config.name = projectName;
        s_activeProject->m_config.assetsDirectoryPath = targetDirPath / "Assets";
        s_activeProject->m_config.startScenePath = targetDirPath / "Assets" / "Scenes" / "Untitled.ekscene";

        std::filesystem::create_directories(s_activeProject->m_config.assetsDirectoryPath);
        std::filesystem::create_directories(targetDirPath / "Assets" / "Scenes");
        std::filesystem::create_directories(targetDirPath / "Assets" / "Meshes");
        std::filesystem::create_directories(targetDirPath / "Assets" / "Materials");
        std::filesystem::create_directories(targetDirPath / "Assets" / "Shaders");
        std::filesystem::create_directories(targetDirPath / "Assets" / "Textures");

        ProjectSerializer serializer(s_activeProject);
        bool success = serializer.Serialize(projectFilePath);
        EK_ASSERT(success, "Failed to serialize project!");

        // TODO: Serialize current editor scene

        return s_activeProject;
    }
    Ref<Project> Project::Load(const std::filesystem::path& projectFilePath)
    {
        Ref<Project> project = CreateRef<Project>();

        ProjectSerializer serializer(project);
        if (serializer.Deserialize(projectFilePath))
        {
            project->m_projectDirectory = projectFilePath.parent_path();
            s_activeProject = project;
            return s_activeProject;
        }

        EK_CORE_WARN("Failed to load project file '{0}'", projectFilePath.string());
        return nullptr;
    }
    bool Project::Save(const std::filesystem::path& projectFilePath)
    {
        ProjectSerializer serializer(s_activeProject);
        if (serializer.Serialize(projectFilePath))
        {
            s_activeProject->m_projectDirectory = projectFilePath.parent_path();
            return true;
        }

        EK_CORE_WARN("Failed to save project file '{0}'", projectFilePath.string());
        return false;
    }
    bool Project::SaveActive()
    {
        return Save(s_activeProject->m_projectDirectory / (s_activeProject->m_config.name + EK_PROJECT_FILE_EXTENSION));
    }
}