#include "precompiled.h"
#include "Timer.h"

#include <chrono>

namespace Eklipse
{
	MainLoopTimer::MainLoopTimer() : m_time(), m_lastTime(), m_deltaTime(0.0f)
	{
		s_instance = this;
		m_time = std::chrono::high_resolution_clock::now();
		m_lastTime = m_time;
	}
	void MainLoopTimer::Record()
	{
		m_time = std::chrono::high_resolution_clock::now();
		m_deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(m_time - m_lastTime).count() / 1000000.0f;
		m_lastTime = m_time;
	}
	float MainLoopTimer::DeltaTime()
	{
		return m_deltaTime;
	}

	Timer::Timer()
	{
		m_startTime = std::chrono::high_resolution_clock::now();
	}
	float Timer::ElapsedTimeMs()
	{
		EK_CORE_PROFILE();
		auto endTime = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count() / 1000.0f;
	}
}