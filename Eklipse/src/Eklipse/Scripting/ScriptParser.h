#pragma once
#include <ScriptAPI/Reflections.h>

namespace Eklipse
{
	using ClassMap = std::unordered_map<std::string, EklipseEngine::Reflections::ClassInfo>;

	class EK_API ScriptParser
	{
	public:
		void ParseDirectory(const std::filesystem::path& directoryPath);

		ClassMap& GetClasses() { return m_classes; }
		void Clear() { m_classes.clear(); }

	private:
		void ParseFile(const std::filesystem::path& filePath);

	private:
		ClassMap m_classes;
	};
}