#pragma once

namespace Eklipse
{
	using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

	class EK_API MainLoopTimer
	{
	public:
		MainLoopTimer();

		void Record();
		float DeltaTime() const;

	private:
		TimePoint m_time;
		TimePoint m_lastTime;
		double m_deltaTime;
	};

	class EK_API Timer
	{
	public:
		Timer();
		float ElapsedTimeMs() const;

		static TimePoint Now();
		static float DurationMs(const TimePoint& start, const TimePoint& end);

	private:
		TimePoint m_startTime;
	};
}