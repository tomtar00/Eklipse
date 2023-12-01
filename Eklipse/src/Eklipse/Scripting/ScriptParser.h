#pragma once
#include <EklipseEngine.h>

namespace Eklipse
{
	class ScriptParser
	{
	public:
		void ParseDirectory(const std::filesystem::path& directoryPath);
		std::unordered_map<std::string, EklipseEngine::ReflectionAPI::ClassInfo>& GetClasses() { return m_classes; }
		void Clear() { m_classes.clear(); }

	private:
		void ParseFile(const std::filesystem::path& filePath);

	private:
		std::unordered_map<std::string, EklipseEngine::ReflectionAPI::ClassInfo> m_classes;
	};
}