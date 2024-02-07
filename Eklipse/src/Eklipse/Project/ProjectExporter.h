#pragma once
#include "Project.h"
#include <Eklipse/Assets/EditorAssetLibrary.h>

namespace Eklipse
{
	struct ProjectExportSettings
	{
		Path path;
		String configuration = "Debug";
	};

	static class EK_API ProjectExporter
	{
	public:
		static bool Export(const Ref<EditorAssetLibrary> assetLibrary, const Ref<Project> project, const ProjectExportSettings& settings);
	};
}