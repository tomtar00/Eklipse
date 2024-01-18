#pragma once

#include <string>

#include "ScriptAPI/Core.h"
#include "ScriptAPI/Script.h"
#include "ScriptAPI/Components.h"
#include "ScriptAPI/Input.h"

namespace EklipseEngine
{
	EK_SCRIPT_API void Log(const std::string& message, ...);
	EK_SCRIPT_API void LogWarn(const std::string& message, ...);
	EK_SCRIPT_API void LogError(const std::string& message, ...);
}