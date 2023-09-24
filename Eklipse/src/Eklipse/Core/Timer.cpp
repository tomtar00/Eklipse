#include "precompiled.h"
#include "Timer.h"

#include <chrono>

namespace Eklipse
{
	Timer::Timer()
	{
		s_instance = this;
	}
	void Timer::Record()
	{
		m_time = std::chrono::high_resolution_clock::now();
		m_deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(m_time - m_lastTime).count() / 1000000.0f;
		m_lastTime = m_time;
	}
	float Timer::DeltaTime()
	{
		return m_deltaTime;
	}
}