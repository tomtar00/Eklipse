#pragma once
#include <filesystem>

#define EK_PROJECT_FILE_EXTENSION ".ekproj"

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

		ProjectConfig& GetConfig() { return m_config; }
		static Ref<Project> GetActive() { return s_activeProject; }
		static const std::filesystem::path& GetProjectDirectory();

		static Ref<Project> New(const std::filesystem::path& targetDirPath, const std::string& projectName);
		static Ref<Project> Load(const std::filesystem::path& projectFilePath);
		static bool Save(const std::filesystem::path& projectFilePath);
		static bool SaveActive();

	private:
		ProjectConfig m_config;
		std::filesystem::path m_projectDirectory;

		static Ref<Project> s_activeProject;
	};
}