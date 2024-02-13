#include "precompiled.h"
#include "ProjectSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Eklipse/Utils/Yaml.h>
#include <filesystem>
#include <Eklipse/Renderer/Renderer.h>

#define abs_to_rel(path) fs::relative(path, config.projectDir).string()
#define rel_to_abs(path) (config.projectDir / path)

#define runtime_abs_to_rel(path) fs::relative(path, runtimeDir).string()
#define runtime_rel_to_abs(path) (runtimeDir / path)

namespace Eklipse
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project) : m_project(project) {}

	bool ProjectSerializer::Serialize(const Path& filepath)
	{
		EK_CORE_PROFILE();
		EK_ASSERT(m_project, "Project is null!");
		const auto& config = m_project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << config.name;

				out << YAML::Key << "AssetsDirectory" << YAML::Value << abs_to_rel(config.assetsDirectoryPath);
				out << YAML::Key << "StartScene" << YAML::Value << abs_to_rel(config.startScenePath);
				out << YAML::Key << "StartSceneHandle" << YAML::Value << config.startSceneHandle;

				out << YAML::Key << "ScriptsDirectory" << YAML::Value << abs_to_rel(config.scriptsDirectoryPath);
				out << YAML::Key << "ScriptsResourcesDirectory" << YAML::Value << abs_to_rel(config.scriptResourcesDirectoryPath);
				out << YAML::Key << "ScriptsGeneratedDirectory" << YAML::Value << abs_to_rel(config.scriptGeneratedDirectoryPath);
				out << YAML::Key << "ScriptsPremakeDirectory" << YAML::Value << abs_to_rel(config.scriptPremakeDirectoryPath);
				out << YAML::Key << "ScriptsBuildDirectory" << YAML::Value << abs_to_rel(config.scriptBuildDirectoryPath);
				out << YAML::Key << "ScriptsSourceDirectory" << YAML::Value << abs_to_rel(config.scriptsSourceDirectoryPath);
				out << YAML::EndMap;
			}

			Renderer::SerializeRendererSettings(out);

			out << YAML::EndMap;
		}

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}
	bool ProjectSerializer::Deserialize(const Path& filepath)
	{
		EK_CORE_PROFILE();
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

		TryDeserailize<String>(projectNode, "Name", &config.name);

		config.projectDir						= filepath.parent_path();

		config.assetsDirectoryPath				= rel_to_abs(TryDeserailize<String>(projectNode, "AssetsDirectory", "Assets"));
		config.startScenePath					= rel_to_abs(TryDeserailize<String>(projectNode, "StartScene", ""));
		config.startSceneHandle                 = TryDeserailize<AssetHandle>(projectNode, "StartSceneHandle", -1);

		config.scriptsDirectoryPath				= rel_to_abs(TryDeserailize<String>(projectNode, "ScriptsDirectory", "Scripts"));
		config.scriptResourcesDirectoryPath		= rel_to_abs(TryDeserailize<String>(projectNode, "ScriptsResourcesDirectory", "Scripts/Resources"));
		config.scriptGeneratedDirectoryPath		= rel_to_abs(TryDeserailize<String>(projectNode, "ScriptsGeneratedDirectory", "Scripts/Resources/Generated"));
		config.scriptPremakeDirectoryPath		= rel_to_abs(TryDeserailize<String>(projectNode, "ScriptsPremakeDirectory", "Scripts/Resources/Premake"));
		config.scriptBuildDirectoryPath			= rel_to_abs(TryDeserailize<String>(projectNode, "ScriptsBuildDirectory", "Scripts/Build"));
		config.scriptsSourceDirectoryPath		= rel_to_abs(TryDeserailize<String>(projectNode, "ScriptsSourceDirectory", "Scripts/Source"));

		auto rendererSettingsNode = data["RendererSettings"];
		if (rendererSettingsNode)
		{
			Renderer::DeserializeRendererSettings(rendererSettingsNode);
		}

		return true;
	}

	bool ProjectSerializer::SerializeRuntimeConfig(const RuntimeConfig& runtimeConfig, const Path& filepath)
	{
		EK_CORE_PROFILE();
		auto& runtimeDir = filepath.parent_path();
		if (runtimeDir.empty())
			runtimeDir = fs::current_path();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Config" << YAML::Value;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << runtimeConfig.name;
			out << YAML::Key << "Executable" << YAML::Value << runtime_abs_to_rel(runtimeConfig.executablePath);
			out << YAML::Key << "AssetsDirectory" << YAML::Value << runtime_abs_to_rel(runtimeConfig.assetsDirectoryPath);
			out << YAML::Key << "ScriptsLibrary" << YAML::Value << runtime_abs_to_rel(runtimeConfig.scriptsLibraryPath);
			out << YAML::Key << "StartScene" << YAML::Value << runtime_abs_to_rel(runtimeConfig.startScenePath);
			out << YAML::Key << "StartSceneHandle" << YAML::Value << runtimeConfig.startSceneHandle;
			out << YAML::EndMap;
		}

		Renderer::SerializeRendererSettings(out);

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}
	bool ProjectSerializer::DeserializeRuntimeConfig(RuntimeConfig& runtimeConfig, const Path& filepath)
	{
		EK_CORE_PROFILE();
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

		auto configNode = data["Config"];
		if (!configNode)
			return false;

		TryDeserailize<String>(configNode, "Name", &runtimeConfig.name);
		runtimeConfig.executablePath		= runtime_rel_to_abs(TryDeserailize<String>(configNode, "Executable", ""));
		runtimeConfig.assetsDirectoryPath	= runtime_rel_to_abs(TryDeserailize<String>(configNode, "AssetsDirectory", "Assets"));
		runtimeConfig.scriptsLibraryPath	= runtime_rel_to_abs(TryDeserailize<String>(configNode, "ScriptsLibrary", ""));
		runtimeConfig.startScenePath		= runtime_rel_to_abs(TryDeserailize<String>(configNode, "StartScene", ""));
		runtimeConfig.startSceneHandle		= TryDeserailize<AssetHandle>(configNode, "StartSceneHandle", -1);

		auto rendererSettingsNode = data["RendererSettings"];
		if (rendererSettingsNode)
		{
			Renderer::DeserializeRendererSettings(rendererSettingsNode);
		}

		return true;
	}
}
