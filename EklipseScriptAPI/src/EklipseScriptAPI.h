#pragma once
#include "ScriptAPI/Reflections.h"

#include <Eklipse.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace EklipseScriptAPI
{
	extern std::shared_ptr<spdlog::logger> Logger;

	struct LoggerConfig
	{
		std::string name;
		std::string pattern;
		spdlog::sink_ptr sink;
	};
	struct ScriptingConfig
	{
		LoggerConfig loggerConfig;
	};

	EK_SCRIPT_API void Init(const ScriptingConfig& config);
}