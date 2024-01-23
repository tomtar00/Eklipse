#pragma once
#include "ClassReflection.h"
#include <ScriptAPI/Reflections.h>

namespace Eklipse
{
	class ScriptParser
	{
	public:
		static std::vector<ClassReflection> ParseDirectory(const std::filesystem::path& directoryPath);
	private:
		static bool ParseFile(const std::filesystem::path& filePath, std::vector<ClassReflection>& outClassReflections);
	};
}