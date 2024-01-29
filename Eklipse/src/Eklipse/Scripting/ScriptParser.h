#pragma once
#include "ClassReflection.h"
#include <ScriptAPI/Reflections.h>

namespace Eklipse
{
	class ScriptParser
	{
	public:
		static Vec<ClassReflection> ParseDirectory(const Path& directoryPath);
	private:
		static bool ParseFile(const Path& filePath, Vec<ClassReflection>& outClassReflections);
	};
}