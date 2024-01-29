#include "precompiled.h"
#include "ScriptParser.h"

namespace Eklipse
{
    Vec<ClassReflection> ScriptParser::ParseDirectory(const Path& directoryPath)
    {
        EK_CORE_TRACE("Parsing directory: {0}", directoryPath.string());

        Vec<ClassReflection> classReflections;
        for (const auto& entry : fs::recursive_directory_iterator(directoryPath))
        {
            if (entry.is_directory())
                continue;
			
            String fileExtension = entry.path().extension().string();
            if (fileExtension == ".h" || fileExtension == ".hpp" || fileExtension == ".cpp")
            {
                Vec<ClassReflection> fileClassReflections{};
                if (ParseFile(entry.path(), fileClassReflections))
                {
                    classReflections.insert(classReflections.end(), fileClassReflections.begin(), fileClassReflections.end());
                }
            }
		}

#ifdef EK_DEBUG
        EK_CORE_TRACE("ScriptParser::ParseDirectory: START = {0}", directoryPath.string());
        for (auto& classRef : classReflections)
        {
			EK_CORE_TRACE("Class: {0}", classRef.className);
            for (auto& memberRef : classRef.members)
            {
				EK_CORE_TRACE("    {0}: type={1}", memberRef.memberName, memberRef.memberType);
			}
        }
        EK_CORE_TRACE("ScriptParser::ParseDirectory: END");
#endif

        EK_CORE_DBG("Parsed directory {0}", directoryPath.string());
        return classReflections;
    }
    bool ScriptParser::ParseFile(const Path& filePath, Vec<ClassReflection>& outClassReflection)
	{
        EK_CORE_TRACE("Parsing file: {0}", filePath.string());
        
        Vec<ClassReflection> classReflections{};

        std::ifstream file(filePath);
        if (file.is_open()) 
        {
            String line;
            std::regex class_regex(R"(class\s+(\w+))");
            std::regex member_regex(R"((\w+)\s+(\w+)\s*(=\s*([^;]+))?;)"); // TODO: support more complex member declarations
            
            ClassReflection classReflection{};
            String currentClassName = "";
            
            while (std::getline(file, line)) 
            {
                std::smatch match;
                if (std::regex_search(line, match, class_regex)) 
                { 
                    if (currentClassName != "")
                    {
						classReflections.push_back(classReflection);
					}

                    currentClassName = match[1].str();
                    classReflection.className = currentClassName;
                    classReflection.members.clear();
                }
                else if (std::regex_search(line, match, member_regex)) 
                {
                    if (currentClassName != "")
                    {
                        ClassMemberReflection memberReflection{};
                        memberReflection.memberName = match[2].str();
                        memberReflection.memberType = match[1].str();
                        classReflection.members.push_back(memberReflection);
                    }
                }
            }

            if (currentClassName != "")
            {
                classReflections.push_back(classReflection);
            }

            file.close();
            return classReflections.size() > 0;
        }
        else
        {
			EK_CORE_ERROR("Failed to open file: {0}", filePath.string());
            return false;
		}
	}
}