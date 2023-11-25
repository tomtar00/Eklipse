#pragma once

#ifdef EK_BUILD_DLL
	#define EK_API __declspec(dllexport)
#else
	#define EK_API __declspec(dllimport)
#endif

namespace Eklipse
{
	class Entity {};
}

namespace EklipseEngine
{
	EK_API class Script
	{
	public:
		virtual void OnCreate() = 0;
		virtual void OnUpdate() = 0;
	protected:
		Eklipse::Entity m_entity;
	};

	EK_API void Log(const char* message);
}