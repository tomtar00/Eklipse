#include "precompiled.h"
#include "ProjectExporter.h"
#include "ProjectSerializer.h"
#include <Eklipse/Scripting/ScriptManager.h>

namespace Eklipse
{
    bool ProjectExporter::Export(const Ref<Project> project, const ProjectExportSettings& settings)
    {
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
        String exportConfig = settings.configuration;

        // Copy assets
        Path assetsDir = config.assetsDirectoryPath;
        Path destinationAssetsDir = destinationDir / "Assets";
        fs::create_directories(destinationAssetsDir);
        for (const auto& entry : fs::recursive_directory_iterator(assetsDir))
        {
            if (entry.is_regular_file())
            {
                Path relativePath = fs::relative(entry.path(), assetsDir);
                Path destinationPath = destinationAssetsDir / relativePath;
                fs::create_directories(destinationPath.parent_path());
                fs::copy_file(entry.path(), destinationPath, fs::copy_options::overwrite_existing);
            }
        }
        runtimeConfig.assetsDirectoryPath = destinationAssetsDir;

        // Copy the scripting library
        Path scriptLibraryPath = config.scriptBuildDirectoryPath / exportConfig / (config.name + EK_SCRIPT_LIBRARY_EXTENSION);
        // Recompile all scripts if the export configuration is different from the current one
        if (scriptLibraryPath.empty() || !fs::exists(scriptLibraryPath))
        {
            EK_CORE_WARN("Script library not found at path '{0}'!", scriptLibraryPath.string());
        }
        else
        {
            Path destinationScriptLibraryPath = destinationDir / (config.name + EK_SCRIPT_LIBRARY_EXTENSION);
            fs::copy_file(scriptLibraryPath, destinationScriptLibraryPath, fs::copy_options::overwrite_existing);
            runtimeConfig.scriptsLibraryPath = destinationScriptLibraryPath;
        }

        // Copy the engine library // TODO: Name shouldnt be const
        Path engineLibraryPath = "Resources/Export/" + exportConfig + "/Eklipse" + EK_SCRIPT_LIBRARY_EXTENSION;
        if (engineLibraryPath.empty() || !fs::exists(engineLibraryPath))
        {
            EK_CORE_ERROR("Engine library not found at path '{0}'!", engineLibraryPath.string());
            return false;
        }
        Path destinationEngineLibraryPath = destinationDir / (String("Eklipse") + EK_SCRIPT_LIBRARY_EXTENSION);
        fs::copy_file(engineLibraryPath, destinationEngineLibraryPath, fs::copy_options::overwrite_existing);

        // Copy the script api library // TODO: Name shouldnt be const
        Path scriptApiLibraryPath = "Resources/Export/" + exportConfig + "/EklipseScriptAPI" + EK_SCRIPT_LIBRARY_EXTENSION;
        if (scriptApiLibraryPath.empty() || !fs::exists(scriptApiLibraryPath))
        {
            EK_CORE_ERROR("Script API library not found at path '{0}'!", scriptApiLibraryPath.string());
            return false;
        }
        Path destinationScriptApiLibraryPath = destinationDir / (String("EklipseScriptAPI") + EK_SCRIPT_LIBRARY_EXTENSION);
        fs::copy_file(scriptApiLibraryPath, destinationScriptApiLibraryPath, fs::copy_options::overwrite_existing);

        // Copy the executable // TODO: Name shouldnt be const
        Path executablePath = Path("Resources/Export") / exportConfig / (String("EklipseRuntime") + EK_EXECUTABLE_EXTENSION);
        if (executablePath.empty() || !fs::exists(executablePath))
        {
            EK_CORE_ERROR("Executable not found at path '{0}'!", executablePath.string());
        }
        Path destinationExecutablePath = destinationDir / (config.name + EK_EXECUTABLE_EXTENSION);
        fs::copy_file(executablePath, destinationExecutablePath, fs::copy_options::overwrite_existing);
        runtimeConfig.executablePath = destinationExecutablePath;

        runtimeConfig.startScenePath = destinationAssetsDir / fs::relative(config.startScenePath, config.assetsDirectoryPath);
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