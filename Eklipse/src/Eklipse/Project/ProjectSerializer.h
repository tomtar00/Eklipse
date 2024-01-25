#pragma once

#include "Project.h"
#include <filesystem>

namespace Eklipse
{
	class EK_API ProjectSerializer
	{
	public:
		ProjectSerializer() = default;
		ProjectSerializer(Ref<Project> project);

		bool Serialize(const Path& filepath);
		bool Deserialize(const Path& filepath);

		bool SerializeRuntimeConfig(const RuntimeConfig& runtimeConfig, const Path& filepath);
		bool DeserializeRuntimeConfig(RuntimeConfig& runtimeConfig, const Path& filepath);

	private:
		Ref<Project> m_project;
	};
}