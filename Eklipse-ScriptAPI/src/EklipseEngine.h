#pragma once

#include "ScriptAPI/Reflections.h"
#include "ScriptAPI/Script.h"
#include "ScriptAPI/Components.h"

namespace EklipseEngine
{
	EK_API void Log(const char* message, ...);
	EK_API void LogWarn(const char* message, ...);
	EK_API void LogError(const char* message, ...);
}