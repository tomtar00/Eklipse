#include "precompiled.h"
#include "Timer.h"

#include <chrono>

namespace Eklipse
{
	MainLoopTimer::MainLoopTimer() : m_time(), m_lastTime(), m_deltaTime(0.0f)
	{
		m_time = Timer::Now();
		m_lastTime = m_time;
	}
	void MainLoopTimer::Record()
	{
		m_time = Timer::Now();
		m_deltaTime = Timer::DurationMs(m_lastTime, m_time) / 1000.0f;
		m_lastTime = m_time;
	}
	float MainLoopTimer::DeltaTime() const
	{
		return m_deltaTime;
	}

	Timer::Timer()
	{
		m_startTime = Timer::Now();
	}
	float Timer::ElapsedTimeMs() const
	{
		EK_CORE_PROFILE();
		auto endTime = Timer::Now();
		return Timer::DurationMs(m_startTime, endTime);
	}

	TimePoint Timer::Now()
	{
		return std::chrono::high_resolution_clock::now();
	}
	float Timer::DurationMs(const TimePoint& start, const TimePoint& end)
	{
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;
	}
}