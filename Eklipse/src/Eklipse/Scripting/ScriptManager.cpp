#include "precompiled.h"
#include "ScriptManager.h"
#include "ScriptParser.h"

#include <Eklipse/Project/Project.h>
#include <Eklipse/Assets/AssetManager.h>
#include <Eklipse/Scene/SceneManager.h>

namespace Eklipse
{
    ScriptManager::ScriptManager(ScriptManagerSettings* settings) 
        : m_settings(settings), m_state(ScriptsState::NONE) 
    {
        m_scriptLinker = CreateRef<ScriptLinker>();
    }

    void ScriptManager::Load()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Loading ScriptModule...");

        EK_ASSERT(Project::GetActive(), "Project is null!");
        auto& config = Project::GetActive()->GetConfig();

        SetState(ScriptsState::NONE);

        auto& libraryPath = config.scriptBuildDirectoryPath / config.configuration / (config.name + EK_SCRIPT_LIBRARY_EXTENSION);
        if (fs::exists(libraryPath) && m_scriptLinker->LinkScriptLibrary(libraryPath))
        {
            auto& classReflections = ScriptParser::ParseDirectory(config.scriptsSourceDirectoryPath);
            m_scriptLinker->FetchScriptClasses(classReflections);
            StartWatchingSource();
        }
        else
        {
            RecompileAll();
        }

        EK_CORE_DBG("ScriptModule loaded");
    }
    void ScriptManager::Unload()
    {
        EK_CORE_PROFILE();
        StopWatchingSource();
        m_scriptLinker->UnlinkScriptLibrary();
    }

    void ScriptManager::StartWatchingSource()
    {
        EK_CORE_PROFILE();
        String sourcePath = Project::GetActive()->GetConfig().scriptsSourceDirectoryPath.string();
        EK_CORE_DBG("ScriptManager::StartWatchingSource: {0}", sourcePath);

        m_sourceWatcher = CreateUnique<filewatch::FileWatch<String>>(sourcePath, CAPTURE_FN(OnSourceWatchEvent));
    }
    void ScriptManager::StopWatchingSource()
    {
        EK_CORE_PROFILE();
        m_sourceWatcher.reset();
    }
    void ScriptManager::OnSourceWatchEvent(const String& path, filewatch::Event change_type)
    {
        String extension = Path(path).extension().string();
        if (extension != ".h" && extension != ".hpp" && extension != ".cpp")
            return;

        if (m_state == ScriptsState::NEEDS_RECOMPILATION)
            return;

        EK_CORE_TRACE("ScriptManager::OnSourceWatchEvent: {0}", path);
        SetState(ScriptsState::NEEDS_RECOMPILATION);

        //if (SceneManager::GetActiveScene()->GetState() != SceneState::RUNNING)
            Application::Get().SubmitToWindowFocus(CAPTURE_FN(RecompileAll));
    }

    bool ScriptManager::GenerateFactoryFile(const Path& targetDirectoryPath, const Vec<ClassReflection>& classReflections)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Generating script factory file at path: {0}", targetDirectoryPath.string());

        if (!fs::exists(targetDirectoryPath))
            fs::create_directories(targetDirectoryPath);

        std::ofstream factoryFile(targetDirectoryPath / "ScriptFactory.cpp");
        factoryFile << "#include <ScriptAPI/Reflections.h>\n";

        // include all script headers
        auto& scriptsSourceDirPath = Project::GetActive()->GetConfig().scriptsSourceDirectoryPath;
        for (const auto& entry : fs::recursive_directory_iterator(scriptsSourceDirPath))
        {
            if (entry.is_directory())
                continue;

            String fileExtension = entry.path().extension().string();
            if (fileExtension == ".h" || fileExtension == ".hpp")
            {
                factoryFile << "#include \"" << fs::relative(entry.path(), scriptsSourceDirPath.parent_path()).string() << "\"\n";
            }
        }

        factoryFile << "using namespace EklipseEngine;\n";
        factoryFile << "using namespace EklipseEngine::Reflections;\n\n";

        factoryFile << "#ifdef EK_PLATFORM_WINDOWS" << "\n"
            << "\t" << "#define EK_EXPORT __declspec(dllexport)" << "\n"
            << "#else" << "\n"
            << "\t" << "#define EK_EXPORT" << "\n"
            << "#endif" << "\n";

        factoryFile << "\n";

        if (classReflections.size() <= 0)
        {
            EK_CORE_DBG("No script classes found!");
            return false;
        }

        factoryFile << "extern \"C\"\n";
        factoryFile << "{\n";

        for (const auto& classRef : classReflections)
        {
            factoryFile << "\t" << "EK_EXPORT void Get__" << classRef.className << "(ClassInfo& info)\n";
            factoryFile << "\t" << "{\n";
            factoryFile << "\t" << "\tinfo.create = [](Ref<Eklipse::Entity> entity)->Script* { auto script = new " << classRef.className << "(); script->SetEntity(entity); return script; };\n";
            factoryFile << "\t" << "\tinfo.reflection.className = \"" << classRef.className << "\";\n";
            for (const auto& [memberName, memberRef] : classRef.members)
            {
                //String memberRefString = fmt::format("\tinfo.reflection.members.push_back({ \"{0}\", \"{1}\", offsetof({2}, {3}) });\n", memberRef.memberName, memberRef.memberType, classRef.className, memberRef.memberName);
                //factoryFile << "\t" << "\tinfo.reflection.members.push_back({ \"" << memberRef.memberName << "\", \"" << memberRef.memberType << "\", offsetof(" << classRef.className << ", " << memberRef.memberName << ") });\n";
                factoryFile << "\t" << "\tinfo.reflection.members[\"" << memberRef.memberName << "\"] = { \"" << memberRef.memberName << "\", \"" << memberRef.memberType << "\", offsetof(" << classRef.className << ", " << memberRef.memberName << ") };\n";
            }
            factoryFile << "\t}\n";
        }
        factoryFile << "}";

        return true;
    }
    void ScriptManager::RunPremake(const Path& premakeDirPath)
    {
        EK_CORE_PROFILE();
        EK_CORE_DBG("Running premake5.lua at path '{}'", premakeDirPath.string());

        auto premakeLuaFilePath = premakeDirPath / "premake5.lua";
        EK_ASSERT(fs::exists(premakeLuaFilePath), "Premake5.lua file does not exist!");
        Path currentPath = fs::current_path();
        EK_ASSERT(fs::exists(currentPath / "Resources/Scripting/Premake/premake5.exe"), "Premake5 executable does not exist!");

#ifdef EK_PLATFORM_WINDOWS
        String command = "cd " + (currentPath / "Resources\\Scripting\\Premake").string() + " && premake5.exe vs2022 --file=" + premakeLuaFilePath.string();
#elif defined(EK_PLATFORM_LINUX)
        String command = "cd " + (currentPath / "Resources/Scripting/Premake").string() + " && premake5 gmake2 --file=" + premakeLuaFilePath.string();
#elif defined(EK_PLATFORM_MACOS)
        String command = "cd " + (currentPath / "Resources/Scripting/Premake").string() + " && premake5 xcode4 --file=" + premakeLuaFilePath.string();
#endif

        EK_CORE_TRACE("Running command: {0}", command);
        int res = system(command.c_str());
        EK_ASSERT(res == 0, "Failed to run premake5.lua!");
    }
    void ScriptManager::CompileScripts(const Path& sourceDirectoryPath, const String& configuration)
    {
        EK_CORE_PROFILE();
        EK_ASSERT(fs::exists(sourceDirectoryPath), "Source directory does not exist!");
        EK_CORE_TRACE("Compiling scripts...");

        String configuration_ = configuration;
        if (configuration_.empty() || (configuration_ != "Debug" && configuration_ != "Release" && configuration_ != "Dist"))
        {
            configuration_ = Project::GetActive()->GetConfig().configuration;
            EK_CORE_WARN("Invalid configuration: {0}. Using default configuration: {1}", configuration, configuration_);
        }

        SetState(ScriptsState::COMPILING);
        String command;

        auto& config = Project::GetActive()->GetConfig();
        auto& projectDirectoryPath = Project::GetActive()->GetProjectDirectory();

#ifdef EK_PLATFORM_WINDOWS

        Path msBuildLocation = m_settings->MsBuildPath;
        if (msBuildLocation.empty() || !fs::is_regular_file(msBuildLocation))
        {
            EK_CORE_ERROR("Failed to locate proper MsBuild executable in location: {}", msBuildLocation.string());
            SetState(ScriptsState::COMPILATION_FAILED);
            return;
        }
        String solutionLocation = (projectDirectoryPath / (config.name + "-Scripts.sln")).string();
        command = msBuildLocation.string() + " /m /p:Configuration=" + configuration_ + " " + solutionLocation;

#elif defined(EK_PLATFORM_LINUX)
        #error Linux compilation not implemented yet
#elif defined(EK_PLATFORM_MACOS)
        #error MacOS compilation not implemented yet
#endif

        EK_CORE_TRACE("Compiling scripts: {0}", command);
        int res = system(command.c_str());
        EK_CORE_TRACE("Compilation result: {0}", res);
        SetState((res == 1) ? ScriptsState::COMPILATION_FAILED : ScriptsState::COMPILATION_SUCCEEDED);
    }
    void ScriptManager::SetState(ScriptsState state)
    {
        EK_CORE_PROFILE();
        m_state = state;
        switch (m_state)
        {
            case ScriptsState::NONE:					m_stateString = "NONE";						break;
            case ScriptsState::COMPILING:				m_stateString = "COMPILING";				break;
            case ScriptsState::COMPILATION_FAILED:		m_stateString = "COMPILATION_FAILED";		break;
            case ScriptsState::COMPILATION_SUCCEEDED:	m_stateString = "COMPILATION_SUCCEEDED";	break;
            case ScriptsState::NEEDS_RECOMPILATION:		m_stateString = "NEEDS_RECOMPILATION";		break;
            default:									m_stateString = "UNKNOWN";					break;
        }
        EK_CORE_TRACE("ScriptModule state changed to {0}", m_stateString);
    }
    void ScriptManager::RecompileAll()
    {
        EK_CORE_PROFILE();
        EK_CORE_INFO("Recompiling scripts...");

        auto& config = Project::GetActive()->GetConfig();

        auto& classReflections = ScriptParser::ParseDirectory(config.scriptsSourceDirectoryPath);
        bool hasCodeToCompile = classReflections.size() > 0;

        auto activeScene = SceneManager::GetActiveScene();
        Path scenePath;
        if (AssetManager::IsAssetHandleValid(activeScene->Handle))
        {
            scenePath = AssetManager::GetMetadata(activeScene->Handle).FilePath;
            Scene::Save(activeScene, scenePath);
            activeScene->DestroyAllScripts();
        }

        Unload();
        GenerateFactoryFile(config.scriptGeneratedDirectoryPath, classReflections);

        if (hasCodeToCompile)
        {
            RunPremake(config.scriptPremakeDirectoryPath);
            CompileScripts(config.scriptsSourceDirectoryPath, config.configuration);

            auto& libraryPath = config.scriptBuildDirectoryPath / config.configuration / (config.name + EK_SCRIPT_LIBRARY_EXTENSION);
            if (FileUtilities::IsPathValid(libraryPath))
            {
                if (m_scriptLinker->LinkScriptLibrary(libraryPath))
                    m_scriptLinker->FetchScriptClasses(classReflections);
                else
                    EK_CORE_ERROR("Library link failed at path: '{0}'. Cannot fetch scripts!", libraryPath.string());
            }
            else
            {
                EK_CORE_ERROR("Library path is invalid: '{0}'", libraryPath.string());
            }

            if (m_state == ScriptsState::COMPILATION_SUCCEEDED)
            {
                if (AssetManager::IsAssetHandleValid(activeScene->Handle))
                {
                    activeScene->InitializeAllScripts(scenePath);
                }
                EK_CORE_INFO("Recompilation successfull!");
            }
            else
            {
                EK_CORE_ERROR("Recompilation failed!");
            }
        }

        StartWatchingSource();
    }
}