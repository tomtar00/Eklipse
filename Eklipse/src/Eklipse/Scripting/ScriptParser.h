#pragma once
#include <ScriptAPI/Reflections.h>

namespace Eklipse
{
    using ClassReflection = EklipseEngine::Reflections::ClassReflection;
	using ClassMemberReflection = EklipseEngine::Reflections::ClassMemberReflection;

	class ScriptParser
	{
	public:
		static Vec<ClassReflection> ParseDirectory(const Path& directoryPath);
	private:
		static bool ParseFile(const Path& filePath, Vec<ClassReflection>& outClassReflections);
	};
}