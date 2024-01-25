#pragma 
#include "ScriptLinker.h"

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
		ScriptManager(Ref<ScriptLinker> scrtiptLinker, ScriptManagerSettings* settings);

		void Load();
		void Unload();

		void RecompileAll();
		bool GenerateFactoryFile(const Path& targetDirectoryPath);
		void RunPremake(const Path& premakeDirPath);
		void CompileScripts(const Path& sourceDirectoryPath, const String& configuration);

		const String& GetState() const { return m_stateString; }
		const ScriptsState GetScriptsState() const { return m_state; }

	private:
		void StartWatchingSource();
		void StopWatchingSource();
		void OnSourceWatchEvent(const String& path, filewatch::Event change_type);

		void SetState(ScriptsState state);
		
	private:
		ScriptManagerSettings* m_settings;
		ScriptsState m_state;
		String m_stateString;

		Ref<ScriptLinker> m_scriptLinker;
		Unique<filewatch::FileWatch<String>> m_sourceWatcher;
	};
}