#pragma once
#include <string>
#include <unordered_map>

#ifdef EK_PLATFORM_WINDOWS
	#ifdef EK_BUILD_DLL
		#define EK_API __declspec(dllexport)
	#else
		#define EK_API __declspec(dllimport)
	#endif
#endif

namespace EklipseEngine
{
	EK_API class Script
	{
	public:
		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;
	};

	EK_API void Log(const char* message);
	EK_API void Close();

	namespace ReflectionAPI
	{
		EK_API struct ClassMemberInfo
		{
			std::string type;
			size_t offset;
		};
		EK_API struct ClassInfo
		{
			Script* (*create)();

			std::string baseClass;
			std::unordered_map<std::string, ClassMemberInfo> members;
		};
	}
}