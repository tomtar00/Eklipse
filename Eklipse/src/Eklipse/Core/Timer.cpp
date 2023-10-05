#include "precompiled.h"
#include "Timer.h"

#include <chrono>

namespace Eklipse
{
	MainLoopTimer::MainLoopTimer()
	{
		s_instance = this;
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
		auto endTime = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count() / 1000.0f;
	}
}