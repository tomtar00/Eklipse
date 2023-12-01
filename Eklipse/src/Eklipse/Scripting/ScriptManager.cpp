#include "precompiled.h"
#include "ScriptManager.h"
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
	void ScriptManager::Load(Ref<Project> project)
	{
		m_sourceWatcher = CreateUnique<filewatch::FileWatch<std::string>>
		(
			project->GetConfig().scriptsSourceDirectoryPath.string(),
			CAPTURE_FN(OnSourceWatchEvent)
		);

		std::string scriptLibraryFilePath = (project->GetConfig().buildDirectoryPath / (project->GetConfig().name + EK_SCRIPT_LIBRARY_EXTENSION)).string();
		if (std::filesystem::exists(scriptLibraryFilePath))
		{
			LinkLibrary(scriptLibraryFilePath);
			FetchFactoryFunctions();
		}
		else
		{
			RecompileAll();
		}
	}
	void ScriptManager::Unload()
	{
		m_library.reset();
		m_libraryWatcher.reset();
		m_sourceWatcher.reset();
	}

	void ScriptManager::OnSourceWatchEvent(const std::string& path, filewatch::Event change_type)
	{
		if (m_state == ScriptsState::NONE || m_state == ScriptsState::COMPILATION_SUCCEEDED)
		{
			EK_CORE_TRACE("ScriptManager::OnSourceWatchEvent: {0}", path);
			std::string extension = std::filesystem::path(path).extension().string();
			if (extension != ".h" && extension != ".hpp" && extension != ".cpp")
				return;

			m_state = ScriptsState::NEEDS_RECOMPILATION;
			Application::Get().SubmitToWindowFocus([this]() 
			{
				RecompileAll();
			});
		}

	}

	void ScriptManager::LinkLibrary(const std::filesystem::path& libraryFilePath)
	{
		try 
		{
			m_library.reset();
			m_library = CreateRef<dylib>(libraryFilePath);
			EK_CORE_INFO("Linked successfully to library: '{0}'", libraryFilePath.string());
		}
		catch (const std::exception e)
		{
			EK_CORE_ERROR("Library link failure at path '{0}'.\n{1}", libraryFilePath.string(), e.what());
		}
	}
	void ScriptManager::GenerateFactoryFile(const std::filesystem::path& targetDirectoryPath)
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

		factoryFile << "using namespace EklipseEngine::ReflectionAPI;\n";
		factoryFile << "\n";

		m_parser.Clear();
		m_parser.ParseDirectory(scriptsSourceDirPath);

		factoryFile << "extern \"C\"\n";
		factoryFile << "{\n";

		// generate script factory functions
		for (const auto& [className, classInfo] : m_parser.GetClasses())
		{
			factoryFile << "\t" << "EK_API void Get__" << className << "(ClassInfo& info)\n";
			factoryFile << "\t" << "{\n";
			factoryFile << "\t" << "	info.create = []()->Script* { return new " << className << "(); };\n";
			for (const auto& [memberName, memberInfo] : classInfo.members)
			{
				factoryFile << "\t" << "	info.members[\"" << memberName << "\"].offset = offsetof(" << className << ", " << memberName << ");\n";
			}
			factoryFile << "\t" << "}\n";
		}

		factoryFile << "}";
	}
	void ScriptManager::CompileScripts(const std::filesystem::path& sourceDirectoryPath)
	{
		EK_ASSERT(std::filesystem::exists(sourceDirectoryPath), "Source directory does not exist!");
		m_state = ScriptsState::COMPILING;
		std::string command;

#ifdef EK_PLATFORM_WINDOWS
		
		// TODO: turn into engine settings variable
		std::string vsLocation = "E:\\Apps\\VisualStudio\\VS2022\\MSBuild\\Current\\Bin\\MSBuild.exe";
		std::string solutionLocation = Project::GetActive()->GetProjectDirectory().string() + "\\" + Project::GetActive()->GetConfig().name + "-Scripts.sln";

		command = vsLocation + " /m /p:Configuration=Debug " + solutionLocation;

		EK_CORE_INFO("dir={0}", vsLocation);
		EK_CORE_INFO("sol={0}", solutionLocation);
		EK_CORE_INFO("cmd={0}", command);

#elif defined(EK_PLATFORM_LINUX)
		#error Linux compilation not implemented yet
#elif defined(EK_PLATFORM_MACOS)
		#error MacOS compilation not implemented yet
#endif

		EK_CORE_TRACE("Compiling scripts: {0}", command);
		int res = system(command.c_str());
		EK_CORE_TRACE("Compilation result: {0}", res);
		m_state = (res == 1) ? ScriptsState::COMPILATION_FAILED : ScriptsState::COMPILATION_SUCCEEDED;
	}
	void ScriptManager::FetchFactoryFunctions()
	{
		EK_ASSERT(m_library, "Script library is not loaded!");

		for (auto&& [className, classInfo] : m_parser.GetClasses())
		{
			try 
			{
				m_library->get_function<void(EklipseEngine::ReflectionAPI::ClassInfo&)>("Get__" + className)(classInfo);
			}
			catch (const std::exception& e) {
				EK_CORE_ERROR("Failed to fetch factory function for class {0}: {1}", className, e.what());
			}
		}
	}
	void ScriptManager::RecompileAll()
	{
		EK_CORE_INFO("Recompiling scripts...");

		const auto& scriptsDirPath = Project::GetActive()->GetConfig().scriptsDirectoryPath;
		GenerateFactoryFile(scriptsDirPath / "Resources" / "Generated");
		CompileScripts(Project::GetActive()->GetConfig().scriptsSourceDirectoryPath);
		if (m_state == ScriptsState::COMPILATION_SUCCEEDED)
		{
			LinkLibrary(
				(Project::GetActive()->GetConfig().buildDirectoryPath / (Project::GetActive()->GetConfig().name + EK_SCRIPT_LIBRARY_EXTENSION))
			);
			FetchFactoryFunctions();

			EK_CORE_INFO("Recompilation finished!");
		}
		else
		{
			EK_CORE_ERROR("Recompilation failed!");
		}
	}
}