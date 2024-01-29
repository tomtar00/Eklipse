#include "precompiled.h"
#include "ScriptLinker.h"

namespace Eklipse
{
	ScriptLinker* ScriptLinker::s_instance = nullptr;

	ScriptLinker::ScriptLinker()
	{
		EK_ASSERT(!s_instance, "ScriptLinker already exists!");
		s_instance = this;
	}

	ScriptLinker& ScriptLinker::Get()
	{
		return *s_instance;
	}

	bool ScriptLinker::LinkScriptLibrary(const Path& libraryPath)
	{
		EK_CORE_TRACE("Loading script library: '{0}'", libraryPath.string());

		try
		{
			m_scriptLibrary = CreateRef<dylib>(libraryPath);
			m_libraryPath = libraryPath;

			EK_CORE_DBG("Linked successfully to library: '{0}'", libraryPath.string());
			return true;
		}
		catch (const std::exception& e)
		{
			EK_CORE_DBG("Library link failure at path '{0}'. {1}", libraryPath.string(), e.what());
			return false;
		}
	}
	bool ScriptLinker::UnlinkScriptLibrary()
	{
		EK_CORE_TRACE("Unloading script library: '{0}'", m_libraryPath.string());

		try
		{
			if (m_scriptLibrary)
			{
				m_scriptLibrary.reset();
			}
			EK_CORE_DBG("Unlinked successfully from library: '{0}'", m_libraryPath.string());
			m_libraryPath = "";
			return true;
		}
		catch (const std::exception& e)
		{
			EK_CORE_ERROR("Library unlink failure. {0}", e.what());
			return false;
		}
	}
	
	void ScriptLinker::ClearScriptClasses()
	{
		m_scriptClassMap.clear();
	}
	void ScriptLinker::FetchScriptClasses(Vec<String> scriptClassNames)
	{
		EK_CORE_TRACE("Fetching script classes...");
		EK_ASSERT(m_scriptLibrary, "Script library is not loaded!");

		for (const auto& scriptClassName : scriptClassNames)
		{
			try 
			{
				ClassInfo classInfo;
				m_scriptLibrary->get_function<void(ClassInfo&)>("Get__" + scriptClassName)(classInfo);
				m_scriptClassMap[scriptClassName] = classInfo;
			}
			catch (const std::exception& e) 
			{
				EK_CORE_ERROR("Failed to fetch script class: {0}. {1}", scriptClassName, e.what());
			}
		}

		EK_CORE_DBG("Script classes fetched!");
	}
	void ScriptLinker::FetchScriptClasses(Vec<ClassReflection> scriptClassReflections)
	{
		Vec<String> scriptClassNames;
		for (const auto& scriptClassReflection : scriptClassReflections)
		{
			scriptClassNames.push_back(scriptClassReflection.className);
		}
		FetchScriptClasses(scriptClassNames);
	}

	bool ScriptLinker::HasAnyScriptClasses() const
	{
		return !m_scriptClassMap.empty();
	}
	bool ScriptLinker::IsLibraryLinked() const
	{
		return m_scriptLibrary != nullptr;
	}

	const Path& ScriptLinker::GetLibraryPath() const
	{
		return m_libraryPath;
	}
	const ScriptClassMap& ScriptLinker::GetScriptClasses() const 
	{
		return m_scriptClassMap;
	}
}