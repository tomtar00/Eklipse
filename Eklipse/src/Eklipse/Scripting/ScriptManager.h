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

	class ScriptManager
	{
	public:
		ScriptManager() = default;
		~ScriptManager() = default;

		void Load(Ref<Project> project);
		void Unload();

		void OnSourceWatchEvent(const std::string& path, filewatch::Event change_type);

		void LinkLibrary(const std::filesystem::path& libraryFilePath);
		void GenerateFactoryFile(const std::filesystem::path& targetDirectoryPath);
		void CompileScripts(const std::filesystem::path& sourceDirectoryPath);
		void FetchFactoryFunctions();

		void RecompileAll();

	private:
		Unique<filewatch::FileWatch<std::string>> m_libraryWatcher;
		Unique<filewatch::FileWatch<std::string>> m_sourceWatcher;

		ScriptsState m_state = ScriptsState::NONE;
		Ref<dylib> m_library;
		ScriptParser m_parser;
	};
}