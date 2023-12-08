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
		Script(size_t entity) : m_entity(entity) {}
		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;

	//protected:
	//	Transform& GetTransform() { return Eklipse::GetTransform(m_entity); }

	private:
		size_t m_entity;
	};

	EK_API void Log(const char* message, ...);
	EK_API void LogWarn(const char* message, ...);
	EK_API void LogError(const char* message, ...);

	namespace Reflections
	{
		EK_API struct ClassMemberInfo
		{
			std::string type;
			size_t offset;
		};
		EK_API struct ClassInfo
		{
			Script* (*create)(size_t entityID);
			std::string baseClass;
			std::unordered_map<std::string, ClassMemberInfo> members;
		};
	}
}