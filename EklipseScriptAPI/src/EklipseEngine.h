#pragma once

#include <string>

#include "ScriptAPI/Core.h"
#include "ScriptAPI/Script.h"
#include "ScriptAPI/Components.h"
#include "ScriptAPI/Input.h"

#define EK_INT int
#define EK_FLOAT float
#define EK_BOOL bool
#define EK_STR std::string
#define EK_VEC2 glm::vec2
#define EK_VEC3 glm::vec3
#define EK_VEC4 glm::vec4

namespace EklipseEngine
{
	EK_SCRIPT_API void Log(const std::string& message, ...);
	EK_SCRIPT_API void LogWarn(const std::string& message, ...);
	EK_SCRIPT_API void LogError(const std::string& message, ...);
}