#include "precompiled.h"
#include "ScriptModule.h"
#include <Eklipse/Project/Project.h>
#include <Eklipse/Core/Application.h>

#ifdef EK_PLATFORM_WINDOWS
	#define EK_SCRIPT_LIBRARY_EXTENSION ".dll"
#elif defined(EK_PLATFORM_LINUX)
	#define EK_SCRIPT_LIBRARY_EXTENSION ".so"
#elif defined(EK_PLATFORM_MACOS)
	#define EK_SCRIPT_LIBRARY_EXTENSION ".dylib"
#endif

namespace Eklipse
{
	void ScriptModule::Load(Ref<Project> project)
	{
		SetState(ScriptsState::NONE);
		m_libraryPath = project->GetConfig().buildDirectoryPath / (project->GetConfig().name + EK_SCRIPT_LIBRARY_EXTENSION);
		if (std::filesystem::exists(m_libraryPath))
		{
			if (LinkLibrary(m_libraryPath))
			{
				m_parser.Clear();
				m_parser.ParseDirectory(project->GetConfig().scriptsSourceDirectoryPath);

				FetchFactoryFunctions();
				StartWatchingSource();
			}
			else
			{
				RecompileAll();
			}
		}
		else
		{
			RecompileAll();
		}
	}
	void ScriptModule::Unload()
	{
		StopWatchingSource();
		UnlinkLibrary();
	}

	void ScriptModule::StartWatchingSource()
	{
		std::string sourcePath = Project::GetActive()->GetConfig().scriptsSourceDirectoryPath.full_string();
		EK_CORE_TRACE("ScriptManager::StartWatchingSource: {0}", sourcePath);

		m_sourceWatcher = CreateUnique<filewatch::FileWatch<std::string>>(sourcePath, CAPTURE_FN(OnSourceWatchEvent));
	}
	void ScriptModule::StopWatchingSource()
	{
		m_sourceWatcher.reset();
	}
	void ScriptModule::OnSourceWatchEvent(const std::string& path, filewatch::Event change_type)
	{
		std::string extension = std::filesystem::path(path).extension().string();
		if (extension != ".h" && extension != ".hpp" && extension != ".cpp")
			return;

		EK_CORE_TRACE("ScriptManager::OnSourceWatchEvent: {0}", path);
		SetState(ScriptsState::NEEDS_RECOMPILATION);
		Application::Get().SubmitToWindowFocus(CAPTURE_FN(RecompileAll));
	}

	bool ScriptModule::LinkLibrary(const std::filesystem::path& libraryFilePath)
	{
		try 
		{
			UnlinkLibrary();
			m_library = CreateRef<dylib>(libraryFilePath);

			EK_CORE_INFO("Linked successfully to library: '{0}'", libraryFilePath.string());
			return true;
		}
		catch (const std::exception e)
		{
			EK_CORE_ERROR("Library link failure at path '{0}'.\n{1}", libraryFilePath.string(), e.what());
			return false;
		}
	}
	void ScriptModule::UnlinkLibrary()
	{
		try
		{
			if (m_library)
			{
				m_library.reset();
				m_parser.Clear();

				EK_CORE_TRACE("Unlinked successfully from library. {0}", m_libraryPath.string());
			}
		}
		catch (const std::exception e)
		{
			EK_CORE_ERROR("Library unlink failure.\n{0}", e.what());
		}
	}

	void ScriptModule::GenerateFactoryFile(const std::filesystem::path& targetDirectoryPath)
	{
		if (!std::filesystem::exists(targetDirectoryPath))
			std::filesystem::create_directories(targetDirectoryPath);

		std::ofstream factoryFile(targetDirectoryPath / "ScriptFactory.cpp");
		factoryFile << "#include <EklipseEngine.h>\n";

		// include all script headers
		auto& scriptsSourceDirPath = Project::GetActive()->GetConfig().scriptsSourceDirectoryPath;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(scriptsSourceDirPath))
		{
			if (entry.is_directory())
				continue;

			std::string fileExtension = entry.path().extension().string();
			if (fileExtension == ".h" || fileExtension == ".hpp")
			{
				factoryFile << "#include \"" << std::filesystem::relative(entry.path(), scriptsSourceDirPath.path().parent_path()).string() << "\"\n";
			}
		}

		factoryFile << "using namespace EklipseEngine;\n";
		factoryFile << "using namespace EklipseEngine::Reflections;\n\n";

		factoryFile << "#ifdef EK_PLATFORM_WINDOWS"							<< "\n"
					<< "\t" << "#define EK_EXPORT __declspec(dllexport)"	<< "\n"
					<< "#else"												<< "\n"
					<< "\t" << "#define EK_EXPORT"							<< "\n"
					<< "#endif"												<< "\n";	

		factoryFile << "\n";

		m_parser.Clear();
		m_parser.ParseDirectory(scriptsSourceDirPath);

		if (m_parser.GetClasses().empty())
		{
			EK_CORE_WARN("No script classes found!");
		}
		else
		{
			factoryFile << "extern \"C\"\n";
			factoryFile << "{\n";

			// generate script export functions
			for (const auto& [className, classInfo] : m_parser.GetClasses())
			{
				// config fill fucntion
				factoryFile << "\t" << "EK_EXPORT void Get__" << className << "(ClassInfo& info)\n";
				factoryFile << "\t" << "{\n";
				factoryFile << "\t" << "	info.create = [](size_t entity)->Script* { return new " << className << "(entity); };\n";
				for (const auto& [memberName, memberInfo] : classInfo.members)
				{
					factoryFile << "\t" << "	info.members[\"" << memberName << "\"].offset = offsetof(" << className << ", " << memberName << ");\n";
				}
				factoryFile << "\t" << "}\n";
			}
			factoryFile << "}";
		}
	}
	void ScriptModule::CompileScripts(const std::filesystem::path& sourceDirectoryPath)
	{
		EK_ASSERT(std::filesystem::exists(sourceDirectoryPath), "Source directory does not exist!");
		SetState(ScriptsState::COMPILING);
		std::string command;

#ifdef EK_PLATFORM_WINDOWS
		
		// TODO: turn into engine settings variable
		std::string vsLocation = "E:\\Apps\\VisualStudio\\VS2022\\MSBuild\\Current\\Bin\\MSBuild.exe";
		std::string solutionLocation = Project::GetActive()->GetProjectDirectory().string() + "\\" + Project::GetActive()->GetConfig().name + "-Scripts.sln";

		command = vsLocation + " /m /p:Configuration=Debug " + solutionLocation;

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
	void ScriptModule::FetchFactoryFunctions()
	{
		EK_ASSERT(m_library, "Script library is not loaded!");

		for (auto&& [className, classInfo] : m_parser.GetClasses())
		{
			try 
			{
				m_library->get_function<void(EklipseEngine::Reflections::ClassInfo&)>("Get__" + className)(classInfo);
			}
			catch (const std::exception& e) 
			{
				EK_CORE_ERROR("Failed to fetch factory function for class {0}: {1}", className, e.what());
			}
		}
	}
	void ScriptModule::SetState(ScriptsState state)
	{
		m_state = state;
		EK_CORE_TRACE("ScriptModule state changed to {0}", (int)m_state);
		switch (m_state)
		{
			case ScriptsState::NONE:					m_stateString = "NONE";						break;
			case ScriptsState::COMPILING:				m_stateString = "COMPILING";				break;
			case ScriptsState::COMPILATION_FAILED:		m_stateString = "COMPILATION_FAILED";		break;
			case ScriptsState::COMPILATION_SUCCEEDED:	m_stateString = "COMPILATION_SUCCEEDED";	break;
			case ScriptsState::NEEDS_RECOMPILATION:		m_stateString = "NEEDS_RECOMPILATION";		break;
			default:									m_stateString = "UNKNOWN";					break;
		}
	}
	void ScriptModule::RecompileAll()
	{
		EK_CORE_INFO("Recompiling scripts...");
		
		Unload();

		GenerateFactoryFile(Project::GetActive()->GetConfig().scriptsDirectoryPath / "Resources" / "Generated");
		CompileScripts(Project::GetActive()->GetConfig().scriptsSourceDirectoryPath);

		if (m_state == ScriptsState::COMPILATION_SUCCEEDED)
		{
			// link to new version of the library	

			EK_CORE_INFO("Recompilation successfull!");
		}
		else
		{
			// link to previous version of the library

			EK_CORE_ERROR("Recompilation failed!");
		}

		if (std::filesystem::exists(m_libraryPath))
		{
			LinkLibrary(m_libraryPath);
			FetchFactoryFunctions();
		}
		else
		{
			EK_CORE_ERROR("Library not found at path: {0}. Cannot fetch scripts!", m_libraryPath.string());
		}

		StartWatchingSource();
	}
}