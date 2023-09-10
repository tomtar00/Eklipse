#pragma once

#include <memory>

#ifdef EK_PLATFORM_WINDOWS
	#ifdef EK_BUILD_DLL
		#define EK_API __declspec(dllexport)
	#else 
		#define EK_API __declspec(dllimport)
	#endif
#else
	#error Engine only supports Windows!
#endif


#define CAPTURE_FN(x) [this](auto&&... args) -> decltype(auto) { return this->x(args...); }

#define BIT(x) (1 << x)
#define NAME_T(x) x
#define STRINGIFY(x) #x

#define EK_ASSERT(x, ...) if (!x) { EK_CORE_CRITICAL("ASSERTION FAILED! {0}", fmt::format(__VA_ARGS__)); __debugbreak(); }

namespace Eklipse
{
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}