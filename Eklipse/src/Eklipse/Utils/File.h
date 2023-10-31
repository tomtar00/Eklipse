#pragma once

namespace Eklipse
{
	extern std::string ReadFileFromPath(const std::string& filename);
	extern void CopyFileContent(const std::string& destination, const std::string& source);
}