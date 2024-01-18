#pragma once

namespace Eklipse
{
	using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

	class EK_API MainLoopTimer
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

	class EK_API Timer
	{
	public:
		Timer();
		float ElapsedTimeMs();

		static TimePoint Now() { return std::chrono::high_resolution_clock::now(); }

	private:
		TimePoint m_startTime;
	};
}