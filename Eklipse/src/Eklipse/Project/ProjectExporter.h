#pragma once
#include "Project.h"

namespace Eklipse
{
	struct ProjectExportSettings
	{
		Path path;
		String configuration = "Debug";
	};

	static class ProjectExporter
	{
	public:
		static bool Export(const Ref<Project> project, const ProjectExportSettings& settings);
	};
}