#pragma once

#include <string>

#include "ScriptAPI/Script.h"
#include "ScriptAPI/Components.h"
#include "ScriptAPI/Input.h"

namespace EklipseEngine
{
	EK_API void Log(const std::string& message, ...);
	EK_API void LogWarn(const std::string& message, ...);
	EK_API void LogError(const std::string& message, ...);
}