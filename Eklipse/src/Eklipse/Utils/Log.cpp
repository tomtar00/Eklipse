#include "precompiled.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Eklipse
{
	EK_API std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	EK_API std::shared_ptr<spdlog::logger> Log::s_clientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%l] [%T:%e] [+%4o ms] %n: %v%$");

		s_coreLogger = spdlog::stderr_color_mt("EKLIPSE");
		s_coreLogger->set_level(spdlog::level::trace);

		s_clientLogger = spdlog::stderr_color_mt("APP");
		s_clientLogger->set_level(spdlog::level::trace);
	}
	std::shared_ptr<spdlog::logger>& Log::coreLogger()
	{
		return s_coreLogger;
	}
	std::shared_ptr<spdlog::logger>& Log::clientLogger()
	{
		return s_clientLogger;
	}
	void Log::AddCoreSink(const std::shared_ptr<spdlog::sinks::sink>& sink)
	{
		s_coreLogger->sinks().push_back(sink);
	}
	void Log::AddClientSink(const std::shared_ptr<spdlog::sinks::sink>& sink)
	{
		s_clientLogger->sinks().push_back(sink);
	}
}