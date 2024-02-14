#include "precompiled.h"
#include "Project.h"
#include "ProjectSerializer.h"

#include <Eklipse/Core/Application.h>
#include <Eklipse/Assets/EditorAssetLibrary.h>

namespace Eklipse
{
	struct ScriptConfig
	{
		String projectName;
		Path includeDir;
		Path libDir;
	};

	static void ReplaceAll(String& str, const String& from, const String& to)
	{
		EK_CORE_PROFILE();
		size_t pos = str.find(from);
		do
		{
			str.replace(pos, from.length(), to);
			pos = str.find(from, pos + to.length());
		} 
		while (pos != String::npos);
	}
	static bool GenerateLuaScript(const String& template_path, const String& output_path, const ScriptConfig& config)
	{
		EK_CORE_PROFILE();
		EK_CORE_TRACE("Generating premake5.lua at path '{0}'", output_path);

		std::ifstream template_file(template_path);
		if (!template_file.is_open())
		{
			EK_ERROR("Failed to open template file '{0}'", template_path);
			return false;
		}
		std::stringstream buffer;
		buffer << template_file.rdbuf();
		String template_content = buffer.str();

		ReplaceAll(template_content, "__PRJ_NAME__", config.projectName);
		ReplaceAll(template_content, "__INCLUDE_DIR__", config.includeDir.generic_string());
		ReplaceAll(template_content, "__LIB_DIR__", config.libDir.generic_string());

		std::ofstream output_file(output_path);
		if (!output_file.is_open())
		{
			EK_ERROR("Failed to open output file '{0}'", output_path);
			return false;
		}
		output_file << template_content;

		EK_CORE_DBG("Generated premake5.lua at path '{0}'", output_path);
		return true;
	}

	Ref<Project> Project::s_activeProject = nullptr;
	Ref<RuntimeConfig> Project::s_runtimeConfig = nullptr;

	Ref<Project> Project::New()
	{
		EK_CORE_PROFILE();
		if (s_activeProject)
		{
			EK_CORE_WARN("Active project already exists!");
		}
		s_activeProject = CreateRef<Project>();
		return s_activeProject;
	}
	Ref<Project> Project::Load(const Path& projectFilePath)
	{
		EK_CORE_PROFILE();
		Ref<Project> project = Project::New();

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
	
	bool Project::SetupActive(const String& name, const Path& projectDirectory)
	{
		EK_CORE_PROFILE();
		s_activeProject->m_projectDirectory = projectDirectory;
		auto& config = s_activeProject->GetConfig();

		if (!FileUtilities::IsPathValid(projectDirectory))
        {
            fs::create_directories(projectDirectory);
        }

		String defaultSceneName = "Untitled";

		config.name = name;
		config.projectDir = projectDirectory;

		config.assetsDirectoryPath = config.projectDir / "Assets";
		config.startScenePath = config.assetsDirectoryPath / "Scenes" / (defaultSceneName + EK_SCENE_EXTENSION);
		config.startSceneHandle = 0;

		config.scriptsDirectoryPath = config.projectDir / "Scripts";
		config.scriptsSourceDirectoryPath = config.scriptsDirectoryPath / "Source";
		config.scriptBuildDirectoryPath = config.scriptsDirectoryPath / "Build";
		config.scriptResourcesDirectoryPath = config.scriptsDirectoryPath / "Resources";
		config.scriptGeneratedDirectoryPath = config.scriptResourcesDirectoryPath / "Generated";
		config.scriptPremakeDirectoryPath = config.scriptResourcesDirectoryPath / "Premake";

		fs::create_directories(config.assetsDirectoryPath / "Scenes");
		fs::create_directories(config.assetsDirectoryPath / "Shaders");
		fs::create_directories(config.assetsDirectoryPath / "Materials");
		fs::create_directories(config.assetsDirectoryPath / "Textures");
		fs::create_directories(config.assetsDirectoryPath / "Meshes");

		fs::create_directories(config.scriptsSourceDirectoryPath);
		fs::create_directories(config.scriptBuildDirectoryPath);
		fs::create_directories(config.scriptPremakeDirectoryPath);
		fs::create_directories(config.scriptBuildDirectoryPath / "Debug");
		fs::create_directories(config.scriptBuildDirectoryPath / "Developement");
		fs::create_directories(config.scriptBuildDirectoryPath / "Release");

		Ref<Scene> startScene = Scene::New(defaultSceneName);
		Scene::Save(startScene, config.startScenePath);

		Path& currentPath = fs::current_path();

		ScriptConfig scriptConfig{};
		scriptConfig.projectName = name;
		scriptConfig.includeDir = currentPath / "Resources/Scripting/Include";
		scriptConfig.libDir = currentPath / "Resources/Export/Debug"; // TODO: Always link to the debug version of the library?

		Path premakeScriptPath = config.scriptPremakeDirectoryPath / "premake5.lua";
		return GenerateLuaScript("Resources/Scripting/Premake/premake5.lua", premakeScriptPath.string(), scriptConfig);
	}
	bool Project::SaveActive()
	{
		EK_CORE_PROFILE();
		return Save(
			s_activeProject,
			s_activeProject->m_projectDirectory / (s_activeProject->m_config.name + EK_PROJECT_EXTENSION)
		);
	}
	bool Project::Save(Ref<Project> project, const Path& projectFilePath)
	{
		EK_CORE_PROFILE();
		ProjectSerializer serializer(project);
		if (serializer.Serialize(projectFilePath))
		{
			project->m_projectDirectory = projectFilePath.parent_path();
			return true;
		}

		EK_CORE_WARN("Failed to save project file '{0}'", projectFilePath.string());
		return false;
	}
	bool Project::Exists(const Path& projectDirPath)
	{
		EK_CORE_PROFILE();
		if (!fs::exists(projectDirPath))
        {
            return false;
        }

		for (const auto& entry : fs::directory_iterator(projectDirPath))
		{
			if (entry.path().extension() == EK_PROJECT_EXTENSION)
			{
				return true;
			}
		}

		return false;
	}
	
	void Project::SetRuntimeConfig(Ref<RuntimeConfig> runtimeConfig)
	{
		s_runtimeConfig = runtimeConfig;
	}
	
	Ref<Project> Project::GetActive()
	{
		return s_activeProject;
	}
	Ref<RuntimeConfig> Project::GetRuntimeConfig()
	{
		return s_runtimeConfig;
	}
	const Path& Project::GetProjectDirectory()
	{
		return m_projectDirectory;
	}
	ProjectConfig& Project::GetConfig()
	{
		return m_config;
	}
}