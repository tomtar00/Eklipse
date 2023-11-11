#include "precompiled.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace Eklipse
{
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