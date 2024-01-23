#pragma once
#include "ScriptParser.h"

#include <dylib.hpp>
#include <FileWatch.hpp>

#include <Eklipse/Core/Timer.h>
#include <Eklipse/Utils/Yaml.h>

#ifdef EK_PLATFORM_WINDOWS
	#define EK_SCRIPT_LIBRARY_EXTENSION ".dll"
#elif defined(EK_PLATFORM_LINUX)
	#define EK_SCRIPT_LIBRARY_EXTENSION ".so"
#elif defined(EK_PLATFORM_MACOS)
	#define EK_SCRIPT_LIBRARY_EXTENSION ".dylib"
#endif

namespace Eklipse
{
	struct ProjectConfig;

	struct ScriptManagerSettings
	{
		Path MsBuildPath;
	};

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
		ScriptManager(ScriptManagerSettings* settings);
		~ScriptManager() = default;

		void Load();
		void Unload();

		void RecompileAll();
		bool GenerateFactoryFile(const std::filesystem::path& targetDirectoryPath);
		void RunPremake(const std::filesystem::path& premakeDirPath);
		void CompileScripts(const std::filesystem::path& sourceDirectoryPath, const std::string& configuration);

		const std::string& GetState() const { return m_stateString; }
		const ScriptsState GetScriptsState() const { return m_state; }

	private:
		void StartWatchingSource();
		void StopWatchingSource();
		void OnSourceWatchEvent(const std::string& path, filewatch::Event change_type);

		void SetState(ScriptsState state);
		
	private:
		ScriptManagerSettings* m_settings;
		ScriptsState m_state;
		std::string m_stateString;

		Unique<filewatch::FileWatch<std::string>> m_sourceWatcher;
	};
}