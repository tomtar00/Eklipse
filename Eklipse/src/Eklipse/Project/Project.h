#pragma once
#include <Eklipse/Assets/AssetLibrary.h>

#define EK_PROJECT_EXTENSION ".ekproj"

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
		String name;
		Path projectDir;

		// assets
		Path assetsDirectoryPath;
		Path startScenePath;
		AssetHandle startSceneHandle;

		// scripts
		Path scriptsDirectoryPath;
		Path scriptResourcesDirectoryPath;
		Path scriptGeneratedDirectoryPath;
		Path scriptPremakeDirectoryPath;
		Path scriptBuildDirectoryPath;
		Path scriptsSourceDirectoryPath;
	};

	struct RuntimeConfig
	{
		String name;
		Path executablePath;
		Path assetsDirectoryPath;
		Path scriptsLibraryPath;
		Path startScenePath;
		AssetHandle startSceneHandle;
	};

	class EK_API Project
	{
	public:
		Project() = default;

		static Ref<Project> New();
		static Ref<Project> Load(const Path& projectFilePath);

		static bool SetupActive(const String& name, const Path& projectDirectory);
		static bool SaveActive();
		static bool Save(Ref<Project> project, const Path& projectFilePath);
		static bool Exists(const Path& projectDirPath);

		static Ref<RuntimeConfig> GetRuntimeConfig();
		static void SetRuntimeConfig(Ref<RuntimeConfig> runtimeConfig);
		static Ref<Project> GetActive();

		const Path& GetProjectDirectory();
		ProjectConfig& GetConfig();

	private:
		Path m_projectDirectory;
		ProjectConfig m_config;

		static Ref<Project> s_activeProject;
		static Ref<RuntimeConfig> s_runtimeConfig;
	};
}