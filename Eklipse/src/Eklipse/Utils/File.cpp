#include "precompiled.h"

namespace Eklipse
{
    std::string ReadFileFromPath(const std::string& filename)
    {
        std::string buffer;
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        EK_ASSERT(file.is_open(), "Failed to open file");

        file.seekg(0, std::ios::end);
        size_t fileSize = (size_t)file.tellg();
        EK_ASSERT(fileSize > 0, "Failed to get file size");

        buffer.resize(fileSize);
        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
}