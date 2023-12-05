#pragma once
#include "ScriptParser.h"

#include <dylib.hpp>
#include <FileWatch.hpp>

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

	private:
		void StartWatchingSource();
		void StopWatchingSource();
		void OnSourceWatchEvent(const std::string& path, filewatch::Event change_type);

		void LinkLibrary(const std::filesystem::path& libraryFilePath);
		void UnlinkLibrary();

		void GenerateFactoryFile(const std::filesystem::path& targetDirectoryPath);
		void CompileScripts(const std::filesystem::path& sourceDirectoryPath);
		void FetchFactoryFunctions();

	private:
		ScriptsState m_state = ScriptsState::NONE;
		Unique<filewatch::FileWatch<std::string>> m_sourceWatcher;

		Ref<dylib> m_library;
		std::filesystem::path m_libraryPath;

		ScriptParser m_parser;
	};
}