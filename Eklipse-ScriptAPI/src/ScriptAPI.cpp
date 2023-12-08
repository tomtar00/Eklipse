#include "ScriptAPI.h"

namespace ScriptAPI
{
	std::shared_ptr<spdlog::logger> Logger;

	static void InitLogger(const ScriptAPI::LoggerConfig& config)
	{
		ScriptAPI::Logger = spdlog::stdout_color_mt(config.name);
		ScriptAPI::Logger->set_pattern(config.pattern);
		ScriptAPI::Logger->sinks().push_back(config.sink);
	}

	EK_API void Init(const ScriptingConfig& config)
	{
		InitLogger(config.loggerConfig);
	}
}