#pragma once
#include <filesystem>

namespace Eklipse
{
	class Path
	{
	public:
		Path() = default;
		Path(const char* path);
		Path(const std::string& path);
		Path(const std::filesystem::path& path);

		static bool CheckPathValid(const Path& path, const std::vector<std::string> requiredExtensions);
		bool IsRelative(const Path& path, Path& base);

		bool isValid() const;
		bool isValid(const std::vector<std::string> requiredExtensions) const;
		void parse(const std::string& path);
		void parseSelf();
		std::string* rdbuf() { return &m_path; }

		uint32_t size() const { return m_path.size(); }
		const bool empty() const { return m_path.empty(); }
		const char* c_str() const { return m_path.c_str(); }
		const char* full_c_str() const { return m_fullPath.c_str(); }
		const std::string& string() const { return m_path; }
		const std::string& full_string() const { return m_fullPath; }
		std::filesystem::path path() const { return std::filesystem::path(m_path); }
		std::filesystem::path full_path() const { return std::filesystem::path(m_fullPath); }

		operator const char*() const { return c_str(); }
		operator std::string() const { return string(); }
		operator std::filesystem::path() const { return full_path(); }

		bool operator==(const Path& other) const		{ return m_fullPath == other.m_fullPath || m_path == other.m_path; }	
		bool operator==(const char* other) const		{ return m_fullPath == other || m_path == other; }
		bool operator==(const std::string other) const	{ return m_fullPath == other || m_path == other; }

		bool operator!=(const Path& other) const		{ return !(*this == other); }
		bool operator!=(const char* other) const		{ return !(*this == other); }
		bool operator!=(const std::string other) const	{ return !(*this == other); }

		Path operator+(const Path& other) const			{ return m_path + other.m_path; }
		Path operator/(const Path& other) const			{ return m_path + "/" + other.m_path; }

	private:
		std::string m_path;
		std::string m_fullPath;
	};

	extern std::string ReadFileFromPath(const Path& filename);
	extern void CopyFileContent(const Path& destination, const Path& source);
}