#pragma once

#ifdef EK_PLATFORM_WINDOWS
	#ifdef EK_BUILD_DLL
		#define EK_API __declspec(dllexport)
	#else 
		#define EK_API __declspec(dllimport)
	#endif
#else
	// Engine only supports Windows!
#endif

#define EK_ASSERT(x, ...) { if (!x) {EK_CORE_CRITICAL("ASSERTION FAILED! {0}", fmt::format(__VA_ARGS__)); exit(-1); }}

#define BIT(x) (1 << x)
#define NAME_T(x) x
#define STRINGIFY(x) #x