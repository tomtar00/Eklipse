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
        if (path.empty())
        {
            m_path = m_fullPath = "";
        }

        m_path = path;
        std::replace(m_path.begin(), m_path.end(), '\\', '/');

        // if is asset relative
        if (m_path.size() > 1 && m_path[0] == '/' && m_path[1] == '/')
        {
            if (Project::GetActive() != nullptr)
                m_fullPath = (Project::GetActive()->GetConfig().assetsDirectoryPath / m_path.substr(2)).string();
            else
                m_fullPath = (Project::GetRuntimeConfig()->assetsDirectoryPath / m_path.substr(2)).string();
        }

        // if is absolute
        else if (m_path.size() > 1 && m_path[1] == ':' && Project::GetActive() != nullptr)
        {
            auto& assetsPath = Project::GetActive()->GetConfig().assetsDirectoryPath;
            if (!assetsPath.empty() && m_path.substr(0, assetsPath.string().size()) == assetsPath)
            {
				m_fullPath = m_path;
                m_path = "/" + m_path.substr(assetsPath.string().size());
			}
            else
            {
                m_fullPath = std::filesystem::absolute(m_path).string();
			}
        }

        // if folder relative
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

    std::string ReadFileFromPath(const std::filesystem::path& filename)
    {
        std::string buffer;
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        EK_ASSERT(file.is_open(), "Failed to open file at '{0}'", filename.string());

        file.seekg(0, std::ios::end);
        size_t fileSize = (size_t)file.tellg();
        EK_ASSERT(fileSize > 0, "Failed to get file size at '{0}'", filename.string());

        buffer.resize(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
    void Eklipse::CopyFileContent(const std::filesystem::path& destination, const std::filesystem::path& source)
    {
        EK_ASSERT(!destination.empty() && !source.empty(), "Copy file error. Source or destination path is empty. destionation={0} source={1}", destination.string(), source.string());
        EK_ASSERT(destination != source, "Copy file error. Source and destination are the same. destionation={0} source={1}", destination.string(), source.string());

        std::ifstream sourceFile(source, std::ios::binary);
        std::ofstream destFile(destination, std::ios::binary);

        destFile << sourceFile.rdbuf();

        EK_CORE_DBG("Copied file content from '{0}' to '{1}'", source.string(), destination.string());
    }
}