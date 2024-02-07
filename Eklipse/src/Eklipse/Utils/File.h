#pragma once
#include <filesystem>

namespace Eklipse
{
	/*class EK_API Path
	{
	public:
		Path() = default;
		Path(const char* path);
		Path(const String& path);
		Path(const Path& path);

		bool IsValid() const;
		bool IsValid(const Vec<String> requiredExtensions) const;
		void Parse(const String& path);
		void ParseSelf();

		const bool IsCurrentlyValid() const { return m_isCurrentlyValid; }
		void SetCurrentlyValid(bool value) { m_isCurrentlyValid = value; }

		String* rdbuf() { return &m_path; }
		uint32_t size() const { return m_path.size(); }
		const bool empty() const { return m_path.empty(); }
		const char* c_str() const { return m_path.c_str(); }
		const char* full_c_str() const { return m_fullPath.c_str(); }
		const String& string() const { return m_path; }
		const String& full_string() const { return m_fullPath; }
		Path path() const { return Path(m_path); }
		Path full_path() const { return Path(m_fullPath); }

		operator const char*() const { return c_str(); }
		operator String() const { return string(); }
		operator Path() const { return full_path(); }

		bool operator==(const Path& other) const		{ return m_fullPath == other.m_fullPath || m_path == other.m_path; }	
		bool operator==(const char* other) const		{ return m_fullPath == other || m_path == other; }
		bool operator==(const String other) const	{ return m_fullPath == other || m_path == other; }

		bool operator!=(const Path& other) const		{ return !(*this == other); }
		bool operator!=(const char* other) const		{ return !(*this == other); }
		bool operator!=(const String other) const	{ return !(*this == other); }

		Path operator+(const Path& other) const			{ return m_path + other.m_path; }
		Path operator/(const Path& other) const			{ return m_path + "/" + other.m_path; }

	private:
		String m_path;
		String m_fullPath;
		bool m_isCurrentlyValid = false;
	};*/

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

	static class EK_API FileUtilities
	{
	public:
		static bool IsPathFile(const Path& path);
		static bool IsPathDirectory(const Path& path);
		static bool IsPathValid(const Path& path);
		static bool IsPathValid(const Path& path, const Vec<String>& extensions);
		static bool ArePathsEqualAndExists(const Path& path1, const Path& path2);
		static bool ArePathsEqual(const Path& path1, const Path& path2);
		static bool IsSubPath(const Path& path, const Path& basePath);

		static String ReadFileFromPath(const Path& filePath);
		static String AppendExtensionIfNotPresent(const String& name, const String& extension);
		static String AppendExtensionIfNotPresent(const Path& path, const String& extension);

		static FileDialogResult OpenFileDialog(const Vec<String>& extensions, const Path& path);
		static FileDialogResult OpenFileDialog(const Vec<String>& extensions);
		static FileDialogResult OpenFileDialog(const Path& path);
		static FileDialogResult OpenFileDialog();

		static FileDialogResult OpenDirDialog(const Path& path);
		static FileDialogResult OpenDirDialog();

	};
}