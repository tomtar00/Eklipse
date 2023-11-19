#include "precompiled.h"
#include "File.h"

#include <Eklipse/Project/Project.h>

namespace Eklipse
{
    Path::Path(const char* path)
    {
        parse(path);
    }
    Path::Path(const std::string& path)
    {
        parse(path);
    }
    Path::Path(const std::filesystem::path& path)
    {
        parse(path.string());
    }
    void Path::parse(const std::string& path)
    {
        m_path = path;
        std::replace(m_path.begin(), m_path.end(), '\\', '/');

        if (m_path.size() >= 2 && m_path[0] == '/' && m_path[1] == '/')
        {
            EK_ASSERT(Project::GetActive() != nullptr, "No project loaded");
            m_fullPath = (Project::GetActive()->GetConfig().assetsDirectoryPath / m_path.substr(2)).full_string();
        }
        else if (Project::GetActive() != nullptr)
        {
            auto& assetsPath = Project::GetActive()->GetConfig().assetsDirectoryPath.full_string();
            if (m_path.substr(0, assetsPath.size()) == assetsPath)
            {
				m_fullPath = m_path;
                m_path = "/" + m_path.substr(assetsPath.size());
			}
            else
            {
                m_fullPath = std::filesystem::absolute(m_path).string();
			}
        }
        else
        {
			m_fullPath = std::filesystem::absolute(m_path).string();
		}
        std::replace(m_fullPath.begin(), m_fullPath.end(), '\\', '/');
    }
    void Path::parseSelf()
    {
        parse(m_path);
    }
    bool Path::isValid() const
    {
		return !m_fullPath.empty() && std::filesystem::exists(m_fullPath);
	}
    bool Path::isValid(const std::vector<std::string> requiredExtensions) const
    {
        bool hasExtension = requiredExtensions.size() == 0;
        for (const auto& requiredExtension : requiredExtensions)
		{
			if (path().extension() == requiredExtension)
			{
				hasExtension = true;
				break;
			}
		}
        return isValid() && hasExtension;
    }
    bool Path::CheckPathValid(const Path& path, const std::vector<std::string> requiredExtensions)
    {
        if (!path.isValid(requiredExtensions))
        {
			EK_CORE_ERROR("Invalid path extension: '{0}'. Required extensions:", path.string());
            for (const auto& requiredExtension : requiredExtensions)
            {
                EK_CORE_ERROR(" - '{0}'", requiredExtension);
            }
			return false;
		}
		return true;
    }

    bool Path::IsRelative(const Path& path, Path& base)
    {
        auto norm_path = std::filesystem::absolute(path);
        auto norm_base = std::filesystem::absolute(base);

        return std::mismatch(norm_path.begin(), norm_path.end(), norm_base.begin()).second == norm_base.end();
    }

    std::string ReadFileFromPath(const Path& filename)
    {
        std::string buffer;
        std::ifstream file(filename.full_c_str(), std::ios::in | std::ios::binary);
        EK_ASSERT(file.is_open(), "Failed to open file at '{0}'", filename);

        file.seekg(0, std::ios::end);
        size_t fileSize = (size_t)file.tellg();
        EK_ASSERT(fileSize > 0, "Failed to get file size at '{0}'", filename);

        buffer.resize(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
    void Eklipse::CopyFileContent(const Path& destination, const Path& source)
    {
        EK_ASSERT(destination != source, "Source and destination are the same");

        std::ifstream sourceFile(source.c_str(), std::ios::binary);
        std::ofstream destFile(destination.c_str(), std::ios::binary);

        destFile << sourceFile.rdbuf();
    }
}