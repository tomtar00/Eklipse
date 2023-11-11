#pragma once
#include <filesystem>
#include <Eklipse/Assets/AssetLibrary.h>

#define EK_PROJECT_FILE_EXTENSION ".ekproj"
#define EK_SCENE_FILE_EXTENSION ".eksc"

namespace Eklipse
{
	struct ProjectConfig
	{
		std::string name;

		std::filesystem::path startScenePath;
		std::filesystem::path assetsDirectoryPath;
	};

	class Project
	{
	public:
		Project();
		inline ProjectConfig& GetConfig() { return m_config; }
		inline const Ref<AssetLibrary> GetAssetLibrary() const { return m_assetLibrary; }
		void LoadAssets();
		void UnloadAssets();

		static Ref<Project> GetActive() { return s_activeProject; }
		static const std::filesystem::path& GetProjectDirectory();

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& projectFilePath);
		static bool Save(Ref<Project> project, const std::filesystem::path& projectFilePath);
		static bool SaveActive();

		static bool Exists(const std::filesystem::path& projectDirPath);

	private:
		ProjectConfig m_config;
		std::filesystem::path m_projectDirectory;
		Ref<AssetLibrary> m_assetLibrary;

		static Ref<Project> s_activeProject;
	};
}