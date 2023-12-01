#pragma once

#include <Eklipse/Core/Core.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace Eklipse
{
	class Log
	{
	public:
		Log() = default;
		
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& coreLogger() { return m_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& clientLogger() { return m_clientLogger; }
		
	private:
		static std::shared_ptr<spdlog::logger> m_coreLogger;
		static std::shared_ptr<spdlog::logger> m_clientLogger;
	};
}

#ifdef EK_DEBUG
	#define EK_CORE_TRACE(...)		SPDLOG_LOGGER_TRACE(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_TEST(...)		SPDLOG_LOGGER_DEBUG(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_INFO(...)		SPDLOG_LOGGER_INFO(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_WARN(...)		SPDLOG_LOGGER_WARN(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Eklipse::Log::coreLogger(), __VA_ARGS__)
	// #define EK_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(Eklipse::Log::coreLogger(), "File: {0}:{1} ERROR: {2}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
	// #define EK_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Eklipse::Log::coreLogger(), "File: {0}:{1} CRITICAL ERROR: {2}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
	
	#define EK_TRACE(...)			SPDLOG_LOGGER_TRACE(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_TEST(...)			SPDLOG_LOGGER_DEBUG(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_INFO(...)			SPDLOG_LOGGER_INFO(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_WARN(...)			SPDLOG_LOGGER_WARN(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_ERROR(...)			SPDLOG_LOGGER_ERROR(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(Eklipse::Log::clientLogger(), __VA_ARGS__)
	//#define EK_ERROR(...)			SPDLOG_LOGGER_ERROR(Eklipse::Log::clientLogger(), "File: {0}:{1} ERROR: {2}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))
	//#define EK_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(Eklipse::Log::clientLogger(), "File: {0}:{1} CRITICAL ERROR: {2}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))

	#define VMA_DEBUG_LOG(format, ...) SPDLOG_LOGGER_TRACE(Eklipse::Log::coreLogger(), format) // TODO: Formatting not working

#elif EK_RELEASE
	#define EK_CORE_TRACE(...)
	#define EK_CORE_TEST(...)		SPDLOG_LOGGER_DEBUG(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_INFO(...)		SPDLOG_LOGGER_INFO(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_WARN(...)		SPDLOG_LOGGER_WARN(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(Eklipse::Log::coreLogger(), __VA_ARGS__)
	#define EK_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Eklipse::Log::coreLogger(), __VA_ARGS__)
	
	#define EK_TRACE(...)
	#define EK_TEST(...)			SPDLOG_LOGGER_DEBUG(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_INFO(...)			SPDLOG_LOGGER_INFO(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_WARN(...)			SPDLOG_LOGGER_WARN(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_ERROR(...)			SPDLOG_LOGGER_ERROR(Eklipse::Log::clientLogger(), __VA_ARGS__)
	#define EK_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(Eklipse::Log::clientLogger(), __VA_ARGS__)

	#define VMA_DEBUG_LOG(format, ...)
#elif EK_DIST
	#define EK_CORE_TRACE(...)
	#define EK_CORE_TEST(...)
	#define EK_CORE_INFO(...)
	#define EK_CORE_WARN(...)
	#define EK_CORE_ERROR(...)
	#define EK_CORE_CRITICAL(...)
	
	#define EK_TRACE(...)	
	#define EK_TEST(...)
	#define EK_INFO(...)
	#define EK_WARN(...)
	#define EK_ERROR(...)
	#define EK_CRITICAL(...)

	#define VMA_DEBUG_LOG(format, ...)
#endif