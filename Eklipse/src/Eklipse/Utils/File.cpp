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
        m_genericPath = path;
        std::replace(m_genericPath.begin(), m_genericPath.end(), '\\', '/');

        m_path = path;
        if (m_path.size() >= 2 && m_path[0] == '/' && m_path[1] == '/')
        {
            EK_ASSERT(Project::GetActive() != nullptr, "No project loaded");
            m_path = (Project::GetActive()->GetConfig().assetsDirectoryPath / m_path.substr(2)).string();
        }
        std::replace(m_path.begin(), m_path.end(), '\\', '/');
    }
    void Path::parseSelf()
    {
        parse(m_genericPath);
    }
    bool Path::isValid() const
    {
		return !m_path.empty() && std::filesystem::exists(m_path);
	}
    bool Path::isValid(const std::string& requiredExtension) const
    {
        return isValid() && path().extension() == requiredExtension;
    }
    bool Path::CheckPathValid(const Path& path, const std::string& requiredExtension)
    {
        if (!path.isValid())
        {
			EK_CORE_ERROR("Invalid path: '{0}'", path.string());
			return false;
		}
        if (!path.isValid(requiredExtension))
        {
			EK_CORE_ERROR("Invalid path extension: '{0}'. Required extension: '{1}'", path.string(), requiredExtension);
			return false;
		}
		return true;
    }

    std::string ReadFileFromPath(const Path& filename)
    {
        std::string buffer;
        std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
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