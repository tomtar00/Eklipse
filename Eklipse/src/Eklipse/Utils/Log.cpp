#include "precompiled.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Eklipse
{
	std::shared_ptr<spdlog::logger> Log::m_coreLogger;
	std::shared_ptr<spdlog::logger> Log::m_clientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%l] [%T:%e] [+%4o ms] %n: %v%$");

		m_coreLogger = spdlog::stderr_color_mt("EKLIPSE");
		m_coreLogger->set_level(spdlog::level::trace);
		EK_CORE_INFO("Eklipse logger initialized");

		m_clientLogger = spdlog::stderr_color_mt("APP");
		m_clientLogger->set_level(spdlog::level::trace);
		EK_CORE_INFO("App logger initialized");
	}
}