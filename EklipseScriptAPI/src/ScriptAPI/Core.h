#pragma once
#include <memory>
#include <filesystem>
#include <string>
#include <vector>

#ifdef EK_PLATFORM_WINDOWS
	#ifdef EK_SCRIPT_API_BUILD_DLL
		#define EK_SCRIPT_API __declspec(dllexport)
	#else
		#define EK_SCRIPT_API __declspec(dllimport)
	#endif
#else
	#error Eklipse only supports Windows!
#endif

namespace EklipseEngine
{
	namespace fs = std::filesystem;
	using Path = fs::path;
	using String = std::string;

	template <typename T>
	using Vec = std::vector<T>;

	template<typename T>
	using Unique = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Unique<T> CreateUnique(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}