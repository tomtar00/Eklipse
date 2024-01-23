#include "precompiled.h"
#include "ScriptManager.h"
#include <Eklipse/Project/Project.h>
#include <Eklipse/Assets/AssetManager.h>

namespace Eklipse
{
	ScriptManager::ScriptManager(ScriptManagerSettings* settings) : m_settings(settings), m_state(ScriptsState::NONE) {}

	void ScriptManager::Load()
	{
		EK_CORE_TRACE("Loading ScriptModule...");

		EK_ASSERT(Project::GetActive(), "Project is null!");
		auto& config = Project::GetActive()->GetConfig();

		SetState(ScriptsState::NONE);

		auto& libraryPath = config.scriptBuildDirectoryPath / config.configuration / (config.name + EK_SCRIPT_LIBRARY_EXTENSION);
		if (std::filesystem::exists(libraryPath) && Application::Get().GetScriptLinker().LinkScriptLibrary(libraryPath))
		{
			auto& classReflections = ScriptParser::ParseDirectory(config.scriptsSourceDirectoryPath);
			Application::Get().GetScriptLinker().FetchScriptClasses(classReflections);
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
		StopWatchingSource();
		Application::Get().GetScriptLinker().UnlinkScriptLibrary();
	}

	void ScriptManager::StartWatchingSource()
	{
		std::string sourcePath = Project::GetActive()->GetConfig().scriptsSourceDirectoryPath.string();
		EK_CORE_DBG("ScriptManager::StartWatchingSource: {0}", sourcePath);

		m_sourceWatcher = CreateUnique<filewatch::FileWatch<std::string>>(sourcePath, CAPTURE_FN(OnSourceWatchEvent));
	}
	void ScriptManager::StopWatchingSource()
	{
		m_sourceWatcher.reset();
	}
	void ScriptManager::OnSourceWatchEvent(const std::string& path, filewatch::Event change_type)
	{
		std::string extension = std::filesystem::path(path).extension().string();
		if (extension != ".h" && extension != ".hpp" && extension != ".cpp")
			return;

		if (m_state == ScriptsState::NEEDS_RECOMPILATION)
			return;

		EK_CORE_TRACE("ScriptManager::OnSourceWatchEvent: {0}", path);
		SetState(ScriptsState::NEEDS_RECOMPILATION);

		if (Application::Get().GetActiveScene()->GetState() != SceneState::RUNNING)
			Application::Get().SubmitToWindowFocus(CAPTURE_FN(RecompileAll));
	}

	bool ScriptManager::GenerateFactoryFile(const std::filesystem::path& targetDirectoryPath)
	{
		EK_CORE_TRACE("Generating script factory file at path: {0}", targetDirectoryPath.string());

		if (!std::filesystem::exists(targetDirectoryPath))
			std::filesystem::create_directories(targetDirectoryPath);

		std::ofstream factoryFile(targetDirectoryPath / "ScriptFactory.cpp");
		factoryFile << "#include <ScriptAPI/Reflections.h>\n";

		// include all script headers
		auto& scriptsSourceDirPath = Project::GetActive()->GetConfig().scriptsSourceDirectoryPath;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(scriptsSourceDirPath))
		{
			if (entry.is_directory())
				continue;

			std::string fileExtension = entry.path().extension().string();
			if (fileExtension == ".h" || fileExtension == ".hpp")
			{
				factoryFile << "#include \"" << std::filesystem::relative(entry.path(), scriptsSourceDirPath.parent_path()).string() << "\"\n";
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

		if (Application::Get().GetScriptLinker().HasAnyScriptClasses())
		{
			EK_CORE_WARN("No script classes found!");
			return false;
		}

		factoryFile << "extern \"C\"\n";
		factoryFile << "{\n";

		for (const auto& [className, classInfo] : Application::Get().GetScriptLinker().GetScriptClasses())
		{
			factoryFile << "\t" << "EK_EXPORT void Get__" << className << "(ClassInfo& info)\n";
			factoryFile << "\t" << "{\n";
			factoryFile << "\t" << "	info.create = [](Ref<Eklipse::Entity> entity)->Script* { auto script = new " << className << "(); script->SetEntity(entity); return script; };\n";
			for (const auto& [memberName, memberInfo] : classInfo.members)
			{
				factoryFile << "\t" << "	info.members[\"" << memberName << "\"] = { " << "\"" << memberInfo.type << "\", " << "offsetof(" << className << ", " << memberName << ") };\n";
			}
			factoryFile << "\t" << "}\n";
		}
		factoryFile << "}";

		return true;
	}
	void ScriptManager::RunPremake(const std::filesystem::path& premakeDirPath)
	{
		EK_CORE_TRACE("Running premake5.lua at path '{}'", premakeDirPath.string());

		auto premakeLuaFilePath = premakeDirPath / "premake5.lua";
		EK_ASSERT(std::filesystem::exists(premakeLuaFilePath), "Premake5.lua file does not exist!");
		std::filesystem::path currentPath = std::filesystem::current_path();
		EK_ASSERT(std::filesystem::exists(currentPath / "Resources/Scripting/Premake/premake5.exe"), "Premake5 executable does not exist!");

#ifdef EK_PLATFORM_WINDOWS
		std::string command = "cd " + (currentPath / "Resources\\Scripting\\Premake").string() + " && premake5.exe vs2022 --file=" + premakeLuaFilePath.string();
#elif defined(EK_PLATFORM_LINUX)
		std::string command = "cd " + (currentPath / "Resources/Scripting/Premake").string() + " && premake5 gmake2 --file=" + premakeLuaFilePath.string();
#elif defined(EK_PLATFORM_MACOS)
		std::string command = "cd " + (currentPath / "Resources/Scripting/Premake").string() + " && premake5 xcode4 --file=" + premakeLuaFilePath.string();
#endif

		EK_CORE_DBG("Running command: {0}", command);
		int res = system(command.c_str());
		EK_ASSERT(res == 0, "Failed to run premake5.lua!");
	}
	void ScriptManager::CompileScripts(const std::filesystem::path& sourceDirectoryPath, const std::string& configuration)
	{
		EK_CORE_TRACE("Compiling scripts...");

		std::string configuration_ = configuration;
		if (configuration_.empty() || (configuration_ != "Debug" && configuration_ != "Release" && configuration_ != "Dist"))
		{
			configuration_ = Project::GetActive()->GetConfig().configuration;
			EK_CORE_WARN("Invalid configuration: {0}. Using default configuration: {1}", configuration, configuration_);
		}

		EK_ASSERT(std::filesystem::exists(sourceDirectoryPath), "Source directory does not exist!");
		SetState(ScriptsState::COMPILING);
		std::string command;

		auto& config = Project::GetActive()->GetConfig();
		auto& projectDirectoryPath = Project::GetActive()->GetProjectDirectory();

#ifdef EK_PLATFORM_WINDOWS

		std::string msBuildLocation = m_settings->MsBuildPath;
		if (msBuildLocation.empty() || !std::filesystem::is_regular_file(msBuildLocation))
		{
			EK_CORE_ERROR("Failed to locate proper MsBuild executable in location: {}", msBuildLocation);
			SetState(ScriptsState::COMPILATION_FAILED);
			return;
		}
		std::string solutionLocation = (projectDirectoryPath / (config.name + "-Scripts.sln")).string();
		command = msBuildLocation + " /m /p:Configuration=" + configuration_ + " " + solutionLocation;

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
		//m_lastStateChangeTime = Timer::Now();
	}
	void ScriptManager::RecompileAll()
	{
		EK_CORE_INFO("Recompiling scripts...");

		auto& config = Project::GetActive()->GetConfig();
		auto& activeScene = Application::Get().GetActiveScene();

		auto& classReflections = ScriptParser::ParseDirectory(config.scriptsSourceDirectoryPath);

		auto& scenePath = AssetManager::GetMetadata(activeScene->Handle).FilePath;
		Scene::Save(activeScene, scenePath);
		activeScene->DestroyAllScripts();

		Unload();

		bool hasCodeToCompile = GenerateFactoryFile(config.scriptGeneratedDirectoryPath);
		if (hasCodeToCompile)
		{
			RunPremake(config.scriptPremakeDirectoryPath);
			CompileScripts(config.scriptsSourceDirectoryPath, config.configuration);

			auto& libraryPath = Application::Get().GetScriptLinker().GetLibraryPath();
			if (std::filesystem::exists(libraryPath) && Application::Get().GetScriptLinker().LinkScriptLibrary(libraryPath))
			{
				Application::Get().GetScriptLinker().FetchScriptClasses(classReflections);
			}
			else
			{
				EK_CORE_ERROR("Library link failed at path: '{0}'. Cannot fetch scripts!", libraryPath.string());
			}

			if (m_state == ScriptsState::COMPILATION_SUCCEEDED)
			{
				activeScene->InitializeAllScripts();
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