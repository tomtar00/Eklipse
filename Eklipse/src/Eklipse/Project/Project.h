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
	#define EK_EXECUTABLE_EXTENSION ".app"
#endif

namespace Eklipse
{
	struct ProjectConfig
	{
		std::string name;
		std::filesystem::path projectDir;

		// platform
		std::filesystem::path msBuildPath;

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

	struct RuntimeConfig
	{
		std::filesystem::path executablePath;
		std::filesystem::path assetsDirectoryPath;
		std::filesystem::path scriptsLibraryPath;
		std::filesystem::path startScenePath;
	};
	struct ProjectExportSettings
	{
		std::string path;
		bool debugBuild;
	};

	class Project
	{
	public:
		Project();
		~Project() = default;
		inline ProjectConfig& GetConfig() { return m_config; }
		inline const Ref<AssetLibrary> GetAssetLibrary() const { return m_assetLibrary; }
		inline ScriptModule& GetScriptModule() { return m_scriptModule; }
		static Ref<Project> GetActive() { return s_activeProject; }
		static const std::filesystem::path& GetProjectDirectory();

		void LoadAssets();
		void UnloadAssets();
		bool Export(const ProjectExportSettings& exportSettings) const;

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& projectFilePath);

		static void SetupActive(const std::string& name, const std::filesystem::path& projectDirectory);
		static bool Save(Ref<Project> project, const std::filesystem::path& projectFilePath);
		static bool SaveActive();
		static bool Exists(const std::filesystem::path& projectDirPath);

		static ClassMap& GetScriptClasses() { return s_activeProject->m_scriptModule.GetClasses(); }
		static Ref<dylib> GetActiveScriptLibrary() { return s_activeProject->m_scriptModule.GetLibrary(); }
		
		static void SetRuntimeConfig(Ref<RuntimeConfig> runtimeConfig) { s_runtimeConfig = runtimeConfig; }
		static Ref<RuntimeConfig> GetRuntimeConfig() { return s_runtimeConfig; }

	private:
		std::filesystem::path m_projectDirectory;
		ProjectConfig m_config;
		Ref<AssetLibrary> m_assetLibrary;
		ScriptModule m_scriptModule;

		static Ref<Project> s_activeProject;
		static Ref<RuntimeConfig> s_runtimeConfig;
	};
}