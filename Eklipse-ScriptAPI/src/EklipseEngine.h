#pragma once

#include "ScriptAPI/Reflections.h"
#include "ScriptAPI/Script.h"
#include "ScriptAPI/Components.h"

namespace EklipseEngine
{
	EK_API void Log(const std::string& message, ...);
	EK_API void LogWarn(const std::string& message, ...);
	EK_API void LogError(const std::string& message, ...);
}