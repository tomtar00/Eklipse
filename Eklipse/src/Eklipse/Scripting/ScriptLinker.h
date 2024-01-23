#pragma once
#include "ClassReflection.h"
#include <ScriptAPI/Reflections.h>

#include <dylib.hpp>

namespace Eklipse
{
	using ClassInfo = EklipseEngine::Reflections::ClassInfo;
	using ScriptClassMap = std::map<std::string, ClassInfo>;

	class ScriptLinker
	{
	public:
		ScriptLinker() = default;

		bool LinkScriptLibrary(const std::filesystem::path& libraryPath);
		bool UnlinkScriptLibrary();

		void ClearScriptClasses();
		void FetchScriptClasses(std::vector<std::string> scriptClassNames);
		void FetchScriptClasses(std::vector<ClassReflection> scriptClassReflections);
		
		bool HasAnyScriptClasses() const;
		const std::filesystem::path& GetLibraryPath() const;
		const ScriptClassMap& GetScriptClasses() const;

	private:
		ScriptClassMap m_scriptClassMap;

		Ref<dylib> m_scriptLibrary;
		std::filesystem::path m_libraryPath;
	};
}