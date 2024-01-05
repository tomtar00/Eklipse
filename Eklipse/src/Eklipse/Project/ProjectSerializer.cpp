#include "precompiled.h"
#include "ProjectSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Eklipse/Utils/Yaml.h>
#include <filesystem>

#define abs_to_rel(path) std::filesystem::relative(path, m_project->GetConfig().projectDir).string()
#define rel_to_abs(path) (m_project->GetConfig().projectDir / path)

namespace Eklipse
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project) : m_project(project) {}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		EK_ASSERT(m_project, "Project is null!");
		const auto& config = m_project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << config.name;

				out << YAML::Key << "MsBuildPath" << YAML::Value << config.msBuildPath.string();

				out << YAML::Key << "StartScene" << YAML::Value << abs_to_rel(config.startScenePath);
				out << YAML::Key << "AssetsDirectory" << YAML::Value << abs_to_rel(config.assetsDirectoryPath);

				out << YAML::Key << "ScriptsDirectory" << YAML::Value << abs_to_rel(config.scriptsDirectoryPath);
				out << YAML::Key << "ScriptsResourcesDirectory" << YAML::Value << abs_to_rel(config.scriptResourcesDirectoryPath);
				out << YAML::Key << "ScriptsGeneratedDirectory" << YAML::Value << abs_to_rel(config.scriptGeneratedDirectoryPath);
				out << YAML::Key << "ScriptsPremakeDirectory" << YAML::Value << abs_to_rel(config.scriptPremakeDirectoryPath);
				out << YAML::Key << "ScriptsBuildDirectory" << YAML::Value << abs_to_rel(config.scriptBuildDirectoryPath);
				out << YAML::Key << "ScriptsSourceDirectory" << YAML::Value << abs_to_rel(config.scriptsSourceDirectoryPath);
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}
	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		EK_ASSERT(m_project, "Project is null!");
		auto& config = m_project->GetConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			EK_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		TryDeserailize<std::string>(projectNode, "Name", &config.name);

		config.projectDir						= filepath.parent_path();

		config.msBuildPath						= TryDeserailize<std::string>(projectNode, "MsBuildPath", "");

		config.startScenePath					= rel_to_abs(TryDeserailize<std::string>(projectNode, "StartScene", ""));
		config.assetsDirectoryPath				= rel_to_abs(TryDeserailize<std::string>(projectNode, "AssetsDirectory", "Assets"));
		config.scriptsDirectoryPath				= rel_to_abs(TryDeserailize<std::string>(projectNode, "ScriptsDirectory", "Scripts"));
		config.scriptResourcesDirectoryPath		= rel_to_abs(TryDeserailize<std::string>(projectNode, "ScriptsResourcesDirectory", ""));
		config.scriptGeneratedDirectoryPath		= rel_to_abs(TryDeserailize<std::string>(projectNode, "ScriptsGeneratedDirectory", ""));
		config.scriptPremakeDirectoryPath		= rel_to_abs(TryDeserailize<std::string>(projectNode, "ScriptsPremakeDirectory", ""));
		config.scriptBuildDirectoryPath			= rel_to_abs(TryDeserailize<std::string>(projectNode, "ScriptsBuildDirectory", ""));
		config.scriptsSourceDirectoryPath		= rel_to_abs(TryDeserailize<std::string>(projectNode, "ScriptsSourceDirectory", ""));

		return true;
	}

	bool ProjectSerializer::SerializeRuntimeConfig(const RuntimeConfig& runtimeConfig, const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Executable" << YAML::Value << runtimeConfig.executablePath.string();
		out << YAML::Key << "AssetsDirectory" << YAML::Value << runtimeConfig.assetsDirectoryPath.string();
		out << YAML::Key << "ScriptsLibrary" << YAML::Value << runtimeConfig.scriptsLibraryPath.string();
		out << YAML::Key << "StartScene" << YAML::Value << runtimeConfig.startScenePath.string();

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}
	bool ProjectSerializer::DeserializeRuntimeConfig(RuntimeConfig& runtimeConfig, const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			EK_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}

		runtimeConfig.executablePath = TryDeserailize<std::string>(data, "Executable", "");
		runtimeConfig.assetsDirectoryPath = TryDeserailize<std::string>(data, "AssetsDirectory", "Assets");
		runtimeConfig.scriptsLibraryPath = TryDeserailize<std::string>(data, "ScriptsLibrary", "");
		runtimeConfig.startScenePath = TryDeserailize<std::string>(data, "StartScene", "");

		return true;
	}
}
