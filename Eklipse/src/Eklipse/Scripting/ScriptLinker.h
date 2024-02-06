#pragma once
#include <ScriptAPI/Reflections.h>
#include <dylib.hpp>

namespace Eklipse
{
	using ClassInfo = EklipseEngine::Reflections::ClassInfo;
	using ClassReflection = EklipseEngine::Reflections::ClassReflection;
	using ScriptClassMap = std::map<String, ClassInfo>;

	class EK_API ScriptLinker
	{
	public:
		ScriptLinker();
		~ScriptLinker();

		static ScriptLinker& Get();

		bool LinkScriptLibrary(const Path& libraryPath);
		bool UnlinkScriptLibrary();

		void ClearScriptClasses();
		void FetchScriptClasses(Vec<String> scriptClassNames);
		void FetchScriptClasses(Vec<ClassReflection> scriptClassReflections);
		
		bool HasAnyScriptClasses() const;
		bool IsLibraryLinked() const;

		const Path& GetLibraryPath() const;
		ScriptClassMap& GetScriptClasses();

	private:
		static ScriptLinker* s_instance;
		ScriptClassMap m_scriptClassMap;

		Ref<dylib> m_scriptLibrary;
		Path m_libraryPath;
	};
}