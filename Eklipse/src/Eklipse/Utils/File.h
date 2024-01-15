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

		bool IsValid() const;
		bool IsValid(const std::vector<std::string> requiredExtensions) const;
		void Parse(const std::string& path);
		void ParseSelf();

		const bool IsCurrentlyValid() const { return m_isCurrentlyValid; }
		void SetCurrentlyValid(bool value) { m_isCurrentlyValid = value; }

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
		bool m_isCurrentlyValid = false;
	};

	extern std::string ReadFileFromPath(const std::filesystem::path& filename);
	extern void CopyFileContent(const std::filesystem::path& destination, const std::filesystem::path& source);

	enum class FileDialogResultType
	{
		SUCCESS = 0,
		FAIL	= 1,
		CANCEL	= 2
	};
	struct FileDialogResult
	{
		FileDialogResultType type;
		Path path;
	};
	extern FileDialogResult OpenFileDialog(const std::vector<std::string>& extensions);
	extern FileDialogResult OpenFileDialog();
	extern FileDialogResult OpenDirDialog();
}