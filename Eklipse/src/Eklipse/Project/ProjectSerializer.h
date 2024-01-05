#pragma once

#include "Project.h"
#include <filesystem>

namespace Eklipse
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer() = default;
		ProjectSerializer(Ref<Project> project);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

		bool SerializeRuntimeConfig(const RuntimeConfig& runtimeConfig, const std::filesystem::path& filepath);
		bool DeserializeRuntimeConfig(RuntimeConfig& runtimeConfig, const std::filesystem::path& filepath);

	private:
		Ref<Project> m_project;
	};
}