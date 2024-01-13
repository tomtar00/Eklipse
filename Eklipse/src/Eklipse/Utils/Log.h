#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace Eklipse
{
	class Log
	{
	public:
		Log() = default;
		
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& coreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& clientLogger() { return s_clientLogger; }

		inline static void AddCoreSink(const std::shared_ptr<spdlog::sinks::sink>& sink) { s_coreLogger->sinks().push_back(sink); }
		inline static void AddClientSink(const std::shared_ptr<spdlog::sinks::sink>& sink) { s_clientLogger->sinks().push_back(sink); }
		
	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};
}

#define THROW(cond, ...) if (!cond) throw std::runtime_error(fmt::format(__VA_ARGS__))

#ifdef EK_DEBUG
	#define EK_CORE_TRACE(...)		SPDLOG_LOGGER_TRACE(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_DBG(...)		SPDLOG_LOGGER_DEBUG(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_INFO(...)		SPDLOG_LOGGER_INFO(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_WARN(...)		SPDLOG_LOGGER_WARN(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Eklipse::Log::coreLogger(), __VA_ARGS__)
	
	#define EK_TRACE(...)			SPDLOG_LOGGER_TRACE(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_DBG(...)			SPDLOG_LOGGER_DEBUG(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_INFO(...)			SPDLOG_LOGGER_INFO(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_WARN(...)			SPDLOG_LOGGER_WARN(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_ERROR(...)			SPDLOG_LOGGER_ERROR(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(Eklipse::Log::clientLogger(), __VA_ARGS__)

	#define VMA_DEBUG_LOG(format, ...) SPDLOG_LOGGER_TRACE(Eklipse::Log::coreLogger(), format) // TODO: Formatting not working

#elif EK_RELEASE
	#define EK_CORE_TRACE(...)
	#define EK_CORE_DBG(...)		SPDLOG_LOGGER_DEBUG(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_INFO(...)		SPDLOG_LOGGER_INFO(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_WARN(...)		SPDLOG_LOGGER_WARN(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Eklipse::Log::coreLogger(), __VA_ARGS__)
	
	#define EK_TRACE(...)
	#define EK_DBG(...)			SPDLOG_LOGGER_DEBUG(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_INFO(...)			SPDLOG_LOGGER_INFO(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_WARN(...)			SPDLOG_LOGGER_WARN(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_ERROR(...)			SPDLOG_LOGGER_ERROR(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(Eklipse::Log::clientLogger(), __VA_ARGS__)

	#define VMA_DEBUG_LOG(format, ...)
#elif EK_DIST
	#define EK_CORE_TRACE(...)
	#define EK_CORE_DBG(...)
	#define EK_CORE_INFO(...)
	#define EK_CORE_WARN(...)
	#define EK_CORE_ERROR(...)
	#define EK_CORE_CRITICAL(...)
	
	#define EK_TRACE(...)	
	#define EK_DBG(...)
	#define EK_INFO(...)
	#define EK_WARN(...)
	#define EK_ERROR(...)
	#define EK_CRITICAL(...)

	#define VMA_DEBUG_LOG(format, ...)
#endif