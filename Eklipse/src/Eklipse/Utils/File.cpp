#include "precompiled.h"
#include "File.h"

namespace Eklipse
{
    std::string ReadFileFromPath(const std::string& filename)
    {
        std::string buffer;
        std::ifstream file(filename, std::ios::in | std::ios::binary);
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
    void Eklipse::CopyFileContent(const std::string& destination, const std::string& source)
    {
        EK_ASSERT(destination != source, "Source and destination are the same");

        std::ifstream sourceFile(source, std::ios::binary);
        std::ofstream destFile(destination, std::ios::binary);

        destFile << sourceFile.rdbuf();
    }
}