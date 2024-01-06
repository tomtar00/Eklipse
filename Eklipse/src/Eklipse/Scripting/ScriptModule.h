#pragma once
#include "ScriptParser.h"

#include <dylib.hpp>
#include <FileWatch.hpp>

#include <Eklipse/Core/Timer.h>

#ifdef EK_PLATFORM_WINDOWS
	#define EK_SCRIPT_LIBRARY_EXTENSION ".dll"
#elif defined(EK_PLATFORM_LINUX)
	#define EK_SCRIPT_LIBRARY_EXTENSION ".so"
#elif defined(EK_PLATFORM_MACOS)
	#define EK_SCRIPT_LIBRARY_EXTENSION ".dylib"
#endif

namespace Eklipse
{
	class Project;

	enum class ScriptsState
	{
		NONE = 0,
		COMPILING,
		COMPILATION_FAILED,
		COMPILATION_SUCCEEDED,
		NEEDS_RECOMPILATION
	};

	class ScriptModule
	{
	public:
		ScriptModule() = default;
		~ScriptModule() = default;

		void Load(Ref<Project> project);
		void Unload();

		bool IsLibraryLoaded() const { return m_library != nullptr; }
		void RecompileAll();

		ClassMap& GetClasses() { return m_parser.GetClasses(); }
		const std::string& GetState() const { return m_stateString; }
		const Ref<dylib>& GetLibrary() const { return m_library; }

		inline TimePoint GetLastStateChangeTime() const { return m_lastStateChangeTime; }

	private:
		void StartWatchingSource();
		void StopWatchingSource();
		void OnSourceWatchEvent(const std::string& path, filewatch::Event change_type);

		bool LinkLibrary(const std::filesystem::path& libraryFilePath);
		void UnlinkLibrary();

		bool GenerateFactoryFile(const std::filesystem::path& targetDirectoryPath);
		void CompileScripts(const std::filesystem::path& sourceDirectoryPath);
		void FetchFactoryFunctions();

		void SetState(ScriptsState state);
		
	private:
		ScriptsState m_state;
		std::string m_stateString;
		TimePoint m_lastStateChangeTime;

		Unique<filewatch::FileWatch<std::string>> m_sourceWatcher;

		Ref<dylib> m_library;
		std::filesystem::path m_libraryPath;

		ScriptParser m_parser;
	};
}