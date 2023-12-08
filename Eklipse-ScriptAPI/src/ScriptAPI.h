#pragma once
#include <Eklipse.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ScriptAPI
{
	extern std::shared_ptr<spdlog::logger> Logger;

	EK_API struct LoggerConfig
	{
		std::string name;
		std::string pattern;
		spdlog::sink_ptr sink;
	};
	EK_API struct ScriptingConfig
	{
		LoggerConfig loggerConfig;
	};

	EK_API void Init(const ScriptingConfig& config);
}