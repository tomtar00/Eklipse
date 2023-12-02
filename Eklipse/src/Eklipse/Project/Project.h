#pragma once
#include <Eklipse/Assets/AssetLibrary.h>
#include <Eklipse/Scripting/ScriptManager.h>

#define EK_PROJECT_FILE_EXTENSION ".ekproj"
#define EK_SCENE_FILE_EXTENSION ".eksc"

namespace Eklipse
{
	struct ProjectConfig
	{
		std::string name;

		Path startScenePath;
		Path assetsDirectoryPath;
		Path buildDirectoryPath;
		Path scriptsDirectoryPath;
		Path scriptsSourceDirectoryPath;
	};

	class Project
	{
	public:
		Project();
		~Project() = default;
		inline ProjectConfig& GetConfig() { return m_config; }
		inline const Ref<AssetLibrary> GetAssetLibrary() const { return m_assetLibrary; }
		static Ref<Project> GetActive() { return s_activeProject; }
		static const std::filesystem::path& GetProjectDirectory();

		void LoadAssets();
		void UnloadAssets();

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& projectFilePath);

		static void SetupActive(const std::string& name, const Path& projectDirectory);
		static bool Save(Ref<Project> project, const std::filesystem::path& projectFilePath);
		static bool SaveActive();

		static bool Exists(const std::filesystem::path& projectDirPath);

		static ClassMap& GetScriptClasses() { return s_activeProject->m_scriptManager.GetClasses(); }

	private:
		std::filesystem::path m_projectDirectory;
		ProjectConfig m_config;
		Ref<AssetLibrary> m_assetLibrary;
		ScriptManager m_scriptManager;

		static Ref<Project> s_activeProject;
	};
}