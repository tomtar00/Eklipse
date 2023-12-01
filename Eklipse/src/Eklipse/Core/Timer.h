#pragma once

namespace Eklipse
{
	using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

	class MainLoopTimer
	{
	public:
		MainLoopTimer();

		void Record();
		float DeltaTime();

	private:
		inline static MainLoopTimer* s_instance = nullptr;

		TimePoint m_time;
		TimePoint m_lastTime;
		double m_deltaTime;
	};

	class Timer
	{
	public:
		Timer();
		float ElapsedTimeMs();

	private:
		TimePoint m_startTime;
	};
}