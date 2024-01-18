#pragma once
#include <Eklipse/Assets/AssetLibrary.h>
#include <Eklipse/Scripting/ScriptModule.h>

#define EK_PROJECT_FILE_EXTENSION ".ekproj"
#define EK_SCENE_FILE_EXTENSION ".eksc"

#ifdef EK_PLATFORM_WINDOWS
	#define EK_EXECUTABLE_EXTENSION ".exe"
#elif defined(EK_PLATFORM_LINUX)
	#define EK_EXECUTABLE_EXTENSION ".exe"
#elif defined(EK_PLATFORM_MACOS)
	#define EK_EXECUTABLE_EXTENSION ".exec"
#endif

namespace Eklipse
{
	struct ProjectConfig
	{
		std::string name;
		std::string configuration;
		std::filesystem::path projectDir;

		// setup
		std::filesystem::path startScenePath;
		std::filesystem::path assetsDirectoryPath;

		// scripts
		std::filesystem::path scriptsDirectoryPath;
		std::filesystem::path scriptResourcesDirectoryPath;
		std::filesystem::path scriptGeneratedDirectoryPath;
		std::filesystem::path scriptPremakeDirectoryPath;
		std::filesystem::path scriptBuildDirectoryPath;
		std::filesystem::path scriptsSourceDirectoryPath;
	};
	struct ProjectSettings
	{
		ScriptModuleSettings* scriptModuleSettings;
	};

	struct RuntimeConfig
	{
		std::filesystem::path executablePath;
		std::filesystem::path assetsDirectoryPath;
		std::filesystem::path scriptsLibraryPath;
		std::filesystem::path startScenePath;
	};
	struct ProjectExportSettings
	{
		Path path;
		bool debugBuild;
	};

	class EK_API Project
	{
	public:
		Project() = delete;
		Project(const ProjectSettings& settings);
		~Project() = default;
		inline ProjectConfig& GetConfig() { return m_config; }
		inline const Ref<AssetLibrary> GetAssetLibrary() const { return m_assetLibrary; }
		inline Ref<ScriptModule> GetScriptModule() { return m_scriptModule; }
		void LoadAssets();
		void UnloadAssets();
		bool Export(const ProjectExportSettings& exportSettings);
		//void ChangeConfiguration(const std::string& configuration);

		static Ref<Project> New(const ProjectSettings& settings);
		static Ref<Project> Load(const std::filesystem::path& projectFilePath, const ProjectSettings& settings);
		static Ref<Project> GetActive() { return s_activeProject; }
		static const std::filesystem::path& GetProjectDirectory();
		static void SetupActive(const std::string& name, const std::filesystem::path& projectDirectory);
		static bool Save(Ref<Project> project, const std::filesystem::path& projectFilePath);
		static bool SaveActive();
		static bool Exists(const std::filesystem::path& projectDirPath);
		static void SetRuntimeConfig(Ref<RuntimeConfig> runtimeConfig) { s_runtimeConfig = runtimeConfig; }
		static Ref<RuntimeConfig> GetRuntimeConfig() { return s_runtimeConfig; }

	private:
		std::filesystem::path m_projectDirectory;
		ProjectConfig m_config;
		Ref<AssetLibrary> m_assetLibrary;
		Ref<ScriptModule> m_scriptModule;

		static Ref<Project> s_activeProject;
		static Ref<RuntimeConfig> s_runtimeConfig;
	};
}