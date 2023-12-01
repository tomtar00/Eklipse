#include "precompiled.h"
#include "ScriptParser.h"

namespace Eklipse
{
    void ScriptParser::ParseDirectory(const std::filesystem::path& directoryPath)
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            if (entry.is_directory())
                continue;
			
            std::string fileExtension = entry.path().extension().string();
            if (fileExtension == ".h" || fileExtension == ".hpp" || fileExtension == ".cpp")
            {
			    ParseFile(entry.path());
            }
		}

#ifdef EK_DEBUG
        EK_CORE_TRACE("ScriptParser::ParseDirectory: START = {0}", directoryPath.string());
        for (auto&& [className, classInfo] : m_classes)
        {
			EK_CORE_TRACE("Class: {0}", className);
            for (auto&& [memberName, memberInfo] : classInfo.members)
            {
				EK_CORE_TRACE("    {0}: type={1} offset={2}", memberName, memberInfo.type, memberInfo.offset);
			}
        }
        EK_CORE_TRACE("ScriptParser::ParseDirectory: END");
#endif
    }
    void ScriptParser::ParseFile(const std::filesystem::path& filePath)
	{
        std::ifstream file(filePath);
        
        if (file.is_open()) 
        {
            std::string line;
            std::regex class_regex(R"(class\s+(\w+)\s+:\s+(\w+))");
            std::regex member_regex(R"((\w+)\s+(\w+);)"); // TODO: support more complex member declarations
            
            std::string currentClassName = "";
            
            while (std::getline(file, line)) 
            {
                std::smatch match;
                if (std::regex_search(line, match, class_regex)) 
                { 
                    currentClassName = match[1].str();
                    auto& classInfo = m_classes[currentClassName];
                    classInfo.baseClass = match[2].str();
                    classInfo.members.clear();
                }
                else if (std::regex_search(line, match, member_regex)) 
                {
                    if (currentClassName != "")
                    {
                        m_classes[currentClassName].members[match[2].str()] = { match[1].str(), 0 };
                    }
                }
            }

            file.close();
        }
        else
        {
			EK_CORE_ERROR("Failed to open file: {0}", filePath.string());
		}
	}
}