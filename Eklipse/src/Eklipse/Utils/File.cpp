#include "precompiled.h"
#include "File.h"

#include <Eklipse/Project/Project.h>
#include <nfd.h>

namespace Eklipse
{
 //   Path::Path(const char* path)
 //   {
 //       Parse(path);
 //       m_isCurrentlyValid = IsValid();
 //   }
 //   Path::Path(const String& path)
 //   {
 //       Parse(path);
 //       m_isCurrentlyValid = IsValid();
 //   }
 //   Path::Path(const Path& path)
 //   {
 //       Parse(path.string());
 //       m_isCurrentlyValid = IsValid();
 //   }
 //   void Path::Parse(const String& path)
 //   {
 //       if (path.empty())
 //       {
 //           m_path = m_fullPath = "";
 //       }

 //       m_path = path;
 //       std::replace(m_path.begin(), m_path.end(), '\\', '/');

 //       // if is asset relative
 //       if (m_path.size() > 1 && m_path[0] == '/' && m_path[1] == '/')
 //       {
 //           if (Project::GetActive() != nullptr)
 //               m_fullPath = (Project::GetActive()->GetConfig().assetsDirectoryPath / m_path.substr(2)).string();
 //           else
 //               m_fullPath = (Project::GetRuntimeConfig()->assetsDirectoryPath / m_path.substr(2)).string();
 //       }

 //       // if is absolute
 //       else if (m_path.size() > 1 && m_path[1] == ':' && Project::GetActive() != nullptr)
 //       {
 //           auto& assetsPath = Project::GetActive()->GetConfig().assetsDirectoryPath;
 //           if (!assetsPath.empty() && m_path.substr(0, assetsPath.string().size()) == assetsPath)
 //           {
	//			m_fullPath = m_path;
 //               m_path = "/" + m_path.substr(assetsPath.string().size());
	//		}
 //           else
 //           {
 //               m_fullPath = fs::absolute(m_path).string();
	//		}
 //       }

 //       // if folder relative
 //       else
 //       {
	//		m_fullPath = fs::absolute(m_path).string();
	//	}
 //       std::replace(m_fullPath.begin(), m_fullPath.end(), '\\', '/');
 //   }
 //   void Path::ParseSelf()
 //   {
 //       Parse(m_path);
 //   }
 //   bool Path::IsValid() const
 //   {
 //       return !m_fullPath.empty() && fs::exists(m_fullPath);
	//}
 //   bool Path::IsValid(const std::vector<String> requiredExtensions) const
 //   {
 //       bool hasExtension = requiredExtensions.size() == 0;
 //       for (const auto& requiredExtension : requiredExtensions)
	//	{
	//		if (path().extension() == requiredExtension)
	//		{
	//			hasExtension = true;
	//			break;
	//		}
	//	}
 //       return IsValid() && hasExtension;
 //   }

    bool IsPathFile(const Path& path)
    {
        return fs::is_regular_file(path);
    }
    bool IsPathDirectory(const Path& path)
    {
        return fs::is_directory(path);
    }
    bool IsPathValid(const Path& path)
    {
        return fs::exists(path);
    }
    bool IsPathValid(const Path& path, std::vector<String> extensions)
    {
        bool hasExtension = false;
        for (const auto& extension : extensions)
        {
            if (path.extension() == extension)
            {
                hasExtension = true;
                break;
            }
		}
        return IsPathValid(path) && hasExtension;
    }

    String ReadFileFromPath(const Path& filePath)
    {
        String buffer;
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        EK_ASSERT(file.is_open(), "Failed to open file at '{0}'", filePath.string());

        file.seekg(0, std::ios::end);
        size_t fileSize = (size_t)file.tellg();
        EK_ASSERT(fileSize > 0, "Failed to get file size at '{0}'", filePath.string());

        buffer.resize(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
    void Eklipse::CopyFileContent(const Path& destination, const Path& source)
    {
        EK_ASSERT(!destination.empty() && !source.empty(), "Copy file error. Source or destination path is empty. destionation={0} source={1}", destination.string(), source.string());
        EK_ASSERT(destination != source, "Copy file error. Source and destination are the same. destionation={0} source={1}", destination.string(), source.string());

        std::ifstream sourceFile(source, std::ios::binary);
        std::ofstream destFile(destination, std::ios::binary);

        destFile << sourceFile.rdbuf();

        EK_CORE_DBG("Copied file content from '{0}' to '{1}'", source.string(), destination.string());
    }
    FileDialogResult OpenFileDialog(const std::vector<String>& extensions)
    {
        String exts = "";
        for (const auto& extension : extensions)
        {
			exts += extension[0] == '.' ? extension.substr(1) : extension;
			exts += ";";
		}

        nfdchar_t* outPath = nullptr;
        nfdresult_t result = NFD_OpenDialog(exts.empty() ? nullptr : exts.c_str(), nullptr, &outPath);

        FileDialogResult res{};
        if (result == NFD_CANCEL)
        {
			res.path = "";
            res.type = FileDialogResultType::CANCEL;
		}
        else if (result == NFD_OKAY)
        {
			res.path = String(outPath);
			res.type = FileDialogResultType::SUCCESS;
		}
        else
        {
			res.path = "";
            res.type = FileDialogResultType::FAIL;
            EK_CORE_ERROR("Failed to open file! {0}", NFD_GetError());
		}
        free(outPath);
        return res;
    }
    FileDialogResult OpenFileDialog()
    {
        return OpenFileDialog({});
    }
    FileDialogResult OpenDirDialog()
    {
        nfdchar_t* outPath = nullptr;
        nfdresult_t result = NFD_PickFolder(nullptr, &outPath);

        FileDialogResult res{};
        if (result == NFD_CANCEL)
        {
            res.path = "";
            res.type = FileDialogResultType::CANCEL;
        }
        else if (result == NFD_OKAY)
        {
			res.path = String(outPath);
			res.type = FileDialogResultType::SUCCESS;
		}
        else
        {
			res.path = "";
			res.type = FileDialogResultType::FAIL;
            EK_CORE_ERROR("Failed to open directory! {0}", NFD_GetError());
		}
        free(outPath);
        return res;
    }
}