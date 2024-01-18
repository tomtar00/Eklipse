#include "precompiled.h"
#include "Project.h"
#include "ProjectSerializer.h"

#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	struct ScriptConfig
	{
		std::string projectName;

		std::filesystem::path includeDir;
		std::filesystem::path libDir;
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
		//replace_all(template_content, "__PRJ_DIR__", config.projectDir);
		replace_all(template_content, "__INCLUDE_DIR__", config.includeDir.string());
		replace_all(template_content, "__LIB_DIR__", config.libDir.string());

		std::ofstream output_file(output_path);
		if (!output_file.is_open())
		{
			EK_ERROR("Failed to open output file '{0}'", output_path);
			return false;
		}
		output_file << template_content;

		return true;
	}

	EK_API Ref<Project> Project::s_activeProject = nullptr;
	EK_API Ref<RuntimeConfig> Project::s_runtimeConfig = nullptr;

	Project::Project(const ProjectSettings& settings)
	{
		m_assetLibrary = CreateRef<AssetLibrary>();
		m_scriptModule = CreateRef<ScriptModule>(settings.scriptModuleSettings);
	}
	void Project::LoadAssets()
	{
		m_assetLibrary->Load(m_config.assetsDirectoryPath);
	}
	void Project::UnloadAssets()
	{
		Renderer::WaitDeviceIdle();
		m_assetLibrary->Unload();
	}
	bool Project::Export(const ProjectExportSettings& exportSettings)
	{
		EK_CORE_INFO("Exporting project '{0}' to '{1}'", m_config.name, exportSettings.path);
		SaveActive();
		Scene::Save(Application::Get().GetActiveScene());
		// TODO: recompile all shaders?

		std::filesystem::path destinationDir = exportSettings.path;
		if (!std::filesystem::is_directory(destinationDir))
		{
			EK_CORE_ERROR("Invalid export path '{0}'! Destination must be a directory.", destinationDir.string());
			return false;
		}

		RuntimeConfig runtimeConfig{};
		std::string exportConfig = exportSettings.debugBuild ? "Debug" : "Dist";

		// Copy assets
		std::filesystem::path assetsDir = m_config.assetsDirectoryPath;
		std::filesystem::path destinationAssetsDir = destinationDir / "Assets";
		std::filesystem::create_directories(destinationAssetsDir);
		for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsDir))
		{
			if (entry.is_regular_file())
			{
				std::filesystem::path relativePath = std::filesystem::relative(entry.path(), assetsDir);
				std::filesystem::path destinationPath = destinationAssetsDir / relativePath;
				std::filesystem::create_directories(destinationPath.parent_path());
				std::filesystem::copy_file(entry.path(), destinationPath, std::filesystem::copy_options::overwrite_existing);
			}
		}
		runtimeConfig.assetsDirectoryPath = destinationAssetsDir;

		// Copy the scripting library
		std::filesystem::path scriptLibraryPath = m_config.scriptBuildDirectoryPath / exportConfig / (m_config.name + EK_SCRIPT_LIBRARY_EXTENSION);
		// Recompile all scripts if the export configuration is different from the current one
		if (m_config.configuration != exportConfig)
		{
			m_scriptModule->CompileScripts(m_config.scriptsSourceDirectoryPath, exportConfig);
		}
		if (scriptLibraryPath.empty() || !std::filesystem::exists(scriptLibraryPath))
		{
			EK_CORE_WARN("Script library not found at path '{0}'!", scriptLibraryPath.string());
		}
		else
		{
			std::filesystem::path destinationScriptLibraryPath = destinationDir / (m_config.name + EK_SCRIPT_LIBRARY_EXTENSION);
			std::filesystem::copy_file(scriptLibraryPath, destinationScriptLibraryPath, std::filesystem::copy_options::overwrite_existing);
			runtimeConfig.scriptsLibraryPath = destinationScriptLibraryPath;
		}

		// Copy the script api library // TODO: Name shouldnt be const
		std::filesystem::path scriptApiLibraryPath = "Resources/Export/" + exportConfig + "/EklipseScriptAPI" + EK_SCRIPT_LIBRARY_EXTENSION;
		if (scriptApiLibraryPath.empty() || !std::filesystem::exists(scriptApiLibraryPath))
		{
			EK_CORE_ERROR("Script API library not found at path '{0}'!", scriptApiLibraryPath.string());
			return false;
		}
		std::filesystem::path destinationScriptApiLibraryPath = destinationDir / (std::string("EklipseScriptAPI") + EK_SCRIPT_LIBRARY_EXTENSION);
		std::filesystem::copy_file(scriptApiLibraryPath, destinationScriptApiLibraryPath, std::filesystem::copy_options::overwrite_existing);

		// Copy the executable // TODO: Name shouldnt be const
		std::filesystem::path executablePath = std::filesystem::path("Resources/Export") / exportConfig / (std::string("EklipseRuntime") + EK_EXECUTABLE_EXTENSION);
		if (executablePath.empty() || !std::filesystem::exists(executablePath))
		{
			EK_CORE_ERROR("Executable not found at path '{0}'!", executablePath.string());
		}
		std::filesystem::path destinationExecutablePath = destinationDir / (m_config.name + EK_EXECUTABLE_EXTENSION);
		std::filesystem::copy_file(executablePath, destinationExecutablePath, std::filesystem::copy_options::overwrite_existing);
		runtimeConfig.executablePath = destinationExecutablePath;

		runtimeConfig.startScenePath = destinationAssetsDir / std::filesystem::relative(m_config.startScenePath, m_config.assetsDirectoryPath);

		// Generate config.yaml file
		ProjectSerializer serializer(s_activeProject);
		if (!serializer.SerializeRuntimeConfig(runtimeConfig, destinationDir / "config.yaml"))
		{
			EK_CORE_ERROR("Failed to generate config.yaml file!");
			return false;
		}

		EK_CORE_INFO("Project exported successfully!");
		return true;
	}

	/*void Project::ChangeConfiguration(const std::string& configuration)
	{
		m_config.configuration = configuration;
		m_scriptModule.Reload();
	}*/

	const std::filesystem::path& Project::GetProjectDirectory()
	{
		EK_ASSERT(s_activeProject != nullptr, "Project is null!");
		return s_activeProject->m_projectDirectory;
	}
	Ref<Project> Project::New(const ProjectSettings& settings)
	{
		return CreateRef<Project>(settings);
	}
	void Project::SetupActive(const std::string& name, const std::filesystem::path& projectDirectory)
	{
		s_activeProject->m_projectDirectory = projectDirectory;
		auto& config = s_activeProject->GetConfig();

		// Create config
		std::string defaultSceneName = "Unititled";
		config.name = name;
		config.configuration = "Debug";
		config.projectDir = projectDirectory;
		config.assetsDirectoryPath = config.projectDir / "Assets";
		config.startScenePath = config.assetsDirectoryPath / "Scenes" / (defaultSceneName + EK_SCENE_FILE_EXTENSION);
		config.scriptsDirectoryPath = config.projectDir / "Scripts";
		config.scriptsSourceDirectoryPath = config.scriptsDirectoryPath / "Source";
		config.scriptBuildDirectoryPath = config.scriptsDirectoryPath / "Build";
		config.scriptResourcesDirectoryPath = config.scriptsDirectoryPath / "Resources";
		config.scriptGeneratedDirectoryPath = config.scriptResourcesDirectoryPath / "Generated";
		config.scriptPremakeDirectoryPath = config.scriptResourcesDirectoryPath / "Premake";

		std::filesystem::create_directories(config.assetsDirectoryPath / "Scenes");
		std::filesystem::create_directories(config.assetsDirectoryPath / "Shaders");
		std::filesystem::create_directories(config.scriptsSourceDirectoryPath);
		std::filesystem::create_directories(config.scriptBuildDirectoryPath);
		std::filesystem::create_directories(config.scriptPremakeDirectoryPath);
		std::filesystem::create_directories(config.scriptBuildDirectoryPath / "Debug");
		std::filesystem::create_directories(config.scriptBuildDirectoryPath / "Release");
		std::filesystem::create_directories(config.scriptBuildDirectoryPath / "Dist");

		// Default 2D shader
		std::filesystem::path dstPath = projectDirectory / "Assets/Shaders/Default2D.eksh";
		Eklipse::CopyFileContent(dstPath, "Assets/Shaders/Default2D.eksh");
		s_activeProject->GetAssetLibrary()->GetShader(dstPath);
		// Default 3D shader
		dstPath = projectDirectory / "Assets/Shaders/Default3D.eksh";
		Eklipse::CopyFileContent(dstPath, "Assets/Shaders/Default3D.eksh");
		s_activeProject->GetAssetLibrary()->GetShader(dstPath);

		// Lua
		auto& currentPath = std::filesystem::current_path();
		ScriptConfig scriptConfig{};
		scriptConfig.projectName = name;
		scriptConfig.includeDir = Path(currentPath / "Resources/Scripting/Include").path();
		scriptConfig.libDir = Path(currentPath / "Resources/Export/Debug").path(); // TODO: Always link to the debug version of the library?

		auto premakeScriptPath = s_activeProject->GetConfig().scriptPremakeDirectoryPath / "premake5.lua";
		bool success = GenerateLuaScript("Resources/Scripting/Premake/premake5.lua", premakeScriptPath.string(), scriptConfig);
		EK_ASSERT(success, "Failed to generate premake5.lua!");
		EK_CORE_INFO("Generated premake5.lua at path '{0}'", premakeScriptPath.string());

		s_activeProject->m_scriptModule->RunPremake(premakeScriptPath);
		s_activeProject->m_scriptModule->Load();
	}
	Ref<Project> Project::Load(const std::filesystem::path& projectFilePath, const ProjectSettings& settings)
	{
		Ref<Project> project = CreateRef<Project>(settings);

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(projectFilePath))
		{
			project->m_projectDirectory = projectFilePath.parent_path();
			s_activeProject = project;

			s_activeProject->m_scriptModule->Load();

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