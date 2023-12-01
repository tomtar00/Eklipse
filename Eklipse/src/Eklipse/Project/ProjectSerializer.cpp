#include "precompiled.h"
#include "ProjectSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Eklipse
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project) : m_project(project) {}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		const auto& config = m_project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << config.name;
				out << YAML::Key << "StartScene" << YAML::Value << config.startScenePath.full_string();
				out << YAML::Key << "AssetsDirectory" << YAML::Value << config.assetsDirectoryPath.full_string();
				out << YAML::Key << "BuildDirectory" << YAML::Value << config.buildDirectoryPath.full_string();
				out << YAML::Key << "ScriptsDirectory" << YAML::Value << config.scriptsDirectoryPath.full_string();
				out << YAML::Key << "ScriptsSourceDirectory" << YAML::Value << config.scriptsSourceDirectoryPath.full_string();
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

		config.name = projectNode["Name"].as<std::string>();
		config.startScenePath = projectNode["StartScene"].as<std::string>();
		config.assetsDirectoryPath = projectNode["AssetsDirectory"].as<std::string>();
		config.buildDirectoryPath = projectNode["BuildDirectory"].as<std::string>();
		config.scriptsDirectoryPath = projectNode["ScriptsDirectory"].as<std::string>();
		config.scriptsSourceDirectoryPath = projectNode["ScriptsSourceDirectory"].as<std::string>();
		return true;
	}
}
