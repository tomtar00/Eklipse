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

		static bool CheckPathValid(const Path& path, const std::string& requiredExtension = "");

		bool isValid() const;
		bool isValid(const std::string& requiredExtension) const;
		void parse(const std::string& path);
		void parseSelf();
		std::string* rdbuf() { return &m_genericPath; }

		uint32_t size() const { return m_path.size(); }
		const bool empty() const { return m_path.empty(); }
		const char* c_str() const { return m_path.c_str(); }
		const std::string& string() const { return m_path; }
		const std::string& generic_string() const { return m_genericPath; }
		std::filesystem::path path() const { return std::filesystem::path(m_path); }

		operator const char*() const { return c_str(); }
		operator std::string() const { return string(); }
		operator std::filesystem::path() const { return path(); }

		bool operator==(const Path& other) const		{ return m_genericPath == other.m_genericPath || m_path == other.m_path; }	
		bool operator==(const char* other) const		{ return m_genericPath == other || m_path == other; }
		bool operator==(const std::string other) const	{ return m_genericPath == other || m_path == other; }

		bool operator!=(const Path& other) const		{ return !(*this == other); }
		bool operator!=(const char* other) const		{ return !(*this == other); }
		bool operator!=(const std::string other) const	{ return !(*this == other); }

		Path operator+(const Path& other) const			{ return m_path + other.m_path; }

	private:
		std::string m_path;
		std::string m_genericPath;
	};

	extern std::string ReadFileFromPath(const Path& filename);
	extern void CopyFileContent(const Path& destination, const Path& source);
}