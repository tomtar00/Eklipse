#include "precompiled.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Eklipse
{
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%l] [%T:%e] [+%4o ms] %n: %v%$");

		s_coreLogger = spdlog::stderr_color_mt("EKLIPSE");
		s_coreLogger->set_level(spdlog::level::trace);
		EK_CORE_INFO("Eklipse logger initialized");

		s_clientLogger = spdlog::stderr_color_mt("APP");
		s_clientLogger->set_level(spdlog::level::trace);
		EK_CORE_INFO("App logger initialized");
	}
}