#pragma once
#include "ClassReflection.h"
#include <ScriptAPI/Reflections.h>

#include <dylib.hpp>

namespace Eklipse
{
	using ClassInfo = EklipseEngine::Reflections::ClassInfo;
	using ScriptClassMap = std::map<String, ClassInfo>;

	class ScriptLinker
	{
	public:
		ScriptLinker() = default;

		bool LinkScriptLibrary(const Path& libraryPath);
		bool UnlinkScriptLibrary();

		void ClearScriptClasses();
		void FetchScriptClasses(std::vector<String> scriptClassNames);
		void FetchScriptClasses(std::vector<ClassReflection> scriptClassReflections);
		
		bool HasAnyScriptClasses() const;
		const Path& GetLibraryPath() const;
		const ScriptClassMap& GetScriptClasses() const;

	private:
		ScriptClassMap m_scriptClassMap;

		Ref<dylib> m_scriptLibrary;
		Path m_libraryPath;
	};
}