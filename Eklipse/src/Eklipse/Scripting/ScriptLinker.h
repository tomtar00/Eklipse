#pragma once
#include "ClassReflection.h"
#include <ScriptAPI/Reflections.h>

#include <dylib.hpp>

namespace Eklipse
{
	using ClassInfo = EklipseEngine::Reflections::ClassInfo;
	using ScriptClassMap = std::map<String, ClassInfo>;

	class EK_API ScriptLinker
	{
	public:
		ScriptLinker();

		static ScriptLinker& Get();

		bool LinkScriptLibrary(const Path& libraryPath);
		bool UnlinkScriptLibrary();

		void ClearScriptClasses();
		void FetchScriptClasses(Vec<String> scriptClassNames);
		void FetchScriptClasses(Vec<ClassReflection> scriptClassReflections);
		
		bool HasAnyScriptClasses() const;
		bool IsLibraryLinked() const;

		const Path& GetLibraryPath() const;
		const ScriptClassMap& GetScriptClasses() const;

	private:
		static ScriptLinker* s_instance;
		ScriptClassMap m_scriptClassMap;

		Ref<dylib> m_scriptLibrary;
		Path m_libraryPath;
	};
}