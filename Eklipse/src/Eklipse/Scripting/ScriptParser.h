#pragma once
#include "ClassReflection.h"
#include <ScriptAPI/Reflections.h>

namespace Eklipse
{
	class ScriptParser
	{
	public:
		static std::vector<ClassReflection> ParseDirectory(const Path& directoryPath);
	private:
		static bool ParseFile(const Path& filePath, std::vector<ClassReflection>& outClassReflections);
	};
}