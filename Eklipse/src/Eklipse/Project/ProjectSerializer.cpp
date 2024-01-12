#include "precompiled.h"
#include "ProjectSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Eklipse/Utils/Yaml.h>
#include <filesystem>

#define abs_to_rel(path) std::filesystem::relative(path, config.projectDir).string()
#define rel_to_abs(path) (config.projectDir / path)

#define runtime_abs_to_rel(path) std::filesystem::relative(path, runtimeDir).string()
#define runtime_rel_to_abs(path) (runtimeDir / path)

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
				out << YAML::Key << "Configuration" << YAML::Value << config.configuration;

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
		TryDeserailize<std::string>(projectNode, "Configuration", &config.configuration);

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
		auto& runtimeDir = filepath.parent_path();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Executable" << YAML::Value << runtime_abs_to_rel(runtimeConfig.executablePath);
		out << YAML::Key << "AssetsDirectory" << YAML::Value << runtime_abs_to_rel(runtimeConfig.assetsDirectoryPath);
		out << YAML::Key << "ScriptsLibrary" << YAML::Value << runtime_abs_to_rel(runtimeConfig.scriptsLibraryPath);
		out << YAML::Key << "StartScene" << YAML::Value << runtime_abs_to_rel(runtimeConfig.startScenePath);

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

		auto& runtimeDir = filepath.parent_path();

		runtimeConfig.executablePath		= runtime_rel_to_abs(TryDeserailize<std::string>(data, "Executable", ""));
		runtimeConfig.assetsDirectoryPath	= runtime_rel_to_abs(TryDeserailize<std::string>(data, "AssetsDirectory", "Assets"));
		runtimeConfig.scriptsLibraryPath	= runtime_rel_to_abs(TryDeserailize<std::string>(data, "ScriptsLibrary", ""));
		runtimeConfig.startScenePath		= runtime_rel_to_abs(TryDeserailize<std::string>(data, "StartScene", ""));

		return true;
	}
}
