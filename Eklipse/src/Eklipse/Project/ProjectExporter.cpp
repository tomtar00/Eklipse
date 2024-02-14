#include "precompiled.h"
#include "ProjectExporter.h"
#include "ProjectSerializer.h"
#include <Eklipse/Scripting/ScriptManager.h>

namespace Eklipse
{
    static String BuildTypeToString(ProjectExportBuildType buildType)
    {
        switch (buildType)
        {
            case ProjectExportBuildType::DEBUG:         return "Debug";
            case ProjectExportBuildType::Developement:  return "Developement";
            case ProjectExportBuildType::Release:       return "Release";
        }
        return "Unknown";
    }

    // TODO: This is a temporary implementation of the project exporter.
    // Is should look for exported scenes and get only the neccessary assets instead of copying everything.
    static bool CopyAssetsFromEditorRegistry(const Path& sourceAssetsDir, const Path& destinationAssetsDir, AssetRegistry& registry)
    {
        EK_CORE_PROFILE();
        AssetRegistry runtimeRegistry;
        fs::create_directories(destinationAssetsDir);
        for (const auto& [handle, matadata] : registry)
        {
            AssetMetadata runtimeMetadata = matadata;
            if (matadata.FilePath.is_absolute())
            {
                runtimeMetadata.FilePath = fs::relative(matadata.FilePath, sourceAssetsDir.parent_path());
                Path destinationPath = destinationAssetsDir.parent_path() / runtimeMetadata.FilePath;
                fs::create_directories(destinationPath.parent_path());

                std::error_code ec;
                fs::copy_file(matadata.FilePath, destinationPath, fs::copy_options::overwrite_existing, ec);
                if (ec)
                {
                    EK_CORE_WARN("Failed to copy file '{0}' to '{1}'! {2}", matadata.FilePath.string(), destinationPath.string(), ec.message());
                    continue;
                }
            }
            else
            {
                Path destinationPath = destinationAssetsDir.parent_path() / matadata.FilePath;
                fs::create_directories(destinationPath.parent_path());

                std::error_code ec;
                fs::copy_file(matadata.FilePath, destinationPath, fs::copy_options::overwrite_existing, ec);
                if (ec)
                {
                    EK_CORE_WARN("Failed to copy file '{0}' to '{1}'! {2}", matadata.FilePath.string(), destinationPath.string(), ec.message());
                    continue;
                }
            }

            runtimeRegistry[handle] = runtimeMetadata;
        }

        return EditorAssetLibrary::SerializeAssetRegistry(runtimeRegistry, destinationAssetsDir / ("assets" + String(EK_REGISTRY_EXTENSION)));
    }

    bool ProjectExporter::Export(const Ref<EditorAssetLibrary> assetLibrary, const Ref<Project> project, const ProjectExportSettings& settings)
    {
        EK_CORE_PROFILE();
        EK_ASSERT(project, "Project is null!");

        auto& config = project->GetConfig();
        EK_CORE_INFO("Exporting project '{0}' to '{1}'", config.name, settings.path.string());

        // TODO: recompile all shaders?

        Path destinationDir = settings.path;
        if (!FileUtilities::IsPathDirectory(destinationDir))
        {
            EK_CORE_ERROR("Invalid export path '{0}'! Destination must be a directory.", destinationDir.string());
            return false;
        }
        
        RuntimeConfig runtimeConfig{};
        runtimeConfig.name = config.name;
        String exportConfig = BuildTypeToString(settings.buildType);

        // Copy assets
        runtimeConfig.assetsDirectoryPath = destinationDir / "Assets";
        if (!CopyAssetsFromEditorRegistry(config.assetsDirectoryPath, runtimeConfig.assetsDirectoryPath, assetLibrary->GetAssetRegistry()))
        {
            EK_CORE_ERROR("Failed to copy assets!");
            return false;
        }

        // Copy the scripting library
        Path scriptLibraryPath = config.scriptBuildDirectoryPath / exportConfig / (config.name + EK_SCRIPT_LIBRARY_EXTENSION);
        // Recompile all scripts if the export configuration is different from the current one
        if (FileUtilities::IsPathValid(scriptLibraryPath))
        {
            Path destinationScriptLibraryPath = destinationDir / (config.name + EK_SCRIPT_LIBRARY_EXTENSION);
            fs::copy_file(scriptLibraryPath, destinationScriptLibraryPath, fs::copy_options::overwrite_existing);
            runtimeConfig.scriptsLibraryPath = destinationScriptLibraryPath;
        }
        else
        {
            EK_CORE_WARN("Script library not found at path '{0}'!", scriptLibraryPath.string());
            return false;
        }

        // Copy the engine library
        Path engineLibraryPath = "Resources/Export/" + exportConfig + "/Eklipse" + EK_SCRIPT_LIBRARY_EXTENSION;
        if (FileUtilities::IsPathValid(engineLibraryPath))
        { 
            Path destinationEngineLibraryPath = destinationDir / (String("Eklipse") + EK_SCRIPT_LIBRARY_EXTENSION);
            fs::copy_file(engineLibraryPath, destinationEngineLibraryPath, fs::copy_options::overwrite_existing);
        }
        else
        {
            EK_CORE_ERROR("Engine library not found at path '{0}'!", engineLibraryPath.string());
            return false;
        }

        // Copy the script api library
        Path scriptApiLibraryPath = "Resources/Export/" + exportConfig + "/EklipseScriptAPI" + EK_SCRIPT_LIBRARY_EXTENSION;
        if (FileUtilities::IsPathValid(scriptApiLibraryPath))
        { 
            Path destinationScriptApiLibraryPath = destinationDir / (String("EklipseScriptAPI") + EK_SCRIPT_LIBRARY_EXTENSION);
            fs::copy_file(scriptApiLibraryPath, destinationScriptApiLibraryPath, fs::copy_options::overwrite_existing);
        }
        else
        {
            EK_CORE_ERROR("Script API library not found at path '{0}'!", scriptApiLibraryPath.string());
            return false;
        }

        // Copy the executable
        Path executablePath = Path("Resources/Export") / exportConfig / (String("EklipseRuntime") + EK_EXECUTABLE_EXTENSION);
        if (FileUtilities::IsPathValid(executablePath))
        { 
            Path destinationExecutablePath = destinationDir / (config.name + EK_EXECUTABLE_EXTENSION);
            fs::copy_file(executablePath, destinationExecutablePath, fs::copy_options::overwrite_existing);
            runtimeConfig.executablePath = destinationExecutablePath;
        }
        else
        {
            EK_CORE_ERROR("Executable not found at path '{0}'!", executablePath.string());
            return false;
        }

        runtimeConfig.startScenePath = runtimeConfig.assetsDirectoryPath / fs::relative(config.startScenePath, config.assetsDirectoryPath);
        runtimeConfig.startSceneHandle = config.startSceneHandle;

        // Generate config.yaml file
        ProjectSerializer serializer(project);
        if (!serializer.SerializeRuntimeConfig(runtimeConfig, destinationDir / "config.yaml"))
        {
            EK_CORE_ERROR("Failed to generate config.yaml file!");
            return false;
        }

        EK_CORE_INFO("Project exported successfully!");
        return true;
    }
}