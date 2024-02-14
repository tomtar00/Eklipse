#pragma once
#include "Project.h"
#include <Eklipse/Assets/EditorAssetLibrary.h>

#ifdef EK_DEBUG
#define EK_CURRENT_CONFIG ProjectExportBuildType::DEBUG
#elif EK_RELEASE
#define EK_CURRENT_CONFIG ProjectExportBuildType::Developement
#elif EK_DIST
#define EK_CURRENT_CONFIG ProjectExportBuildType::Release
#else 
#error "No configuration specified!"
#endif

namespace Eklipse
{
	enum class ProjectExportBuildType
	{
        DEBUG = 0,			// EK_DEBUG
        Developement = 1,	// EK_RELEASE
		Release = 2,		// EK_DIST
    };
	struct ProjectExportSettings
	{
		Path path;
		ProjectExportBuildType buildType;
	};

	static class EK_API ProjectExporter
	{
	public:
		static bool Export(const Ref<EditorAssetLibrary> assetLibrary, const Ref<Project> project, const ProjectExportSettings& settings);
	};
}