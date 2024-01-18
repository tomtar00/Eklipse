#include "precompiled.h"
#include "EklipseScriptAPI.h"

namespace EklipseScriptAPI
{
	std::shared_ptr<spdlog::logger> Logger;

	static void InitLogger(const LoggerConfig& config)
	{
		Logger = spdlog::stdout_color_mt(config.name);
		Logger->set_pattern(config.pattern);
		Logger->sinks().push_back(config.sink);
	}

	void Init(const ScriptingConfig& config)
	{
		InitLogger(config.loggerConfig);
	}
}