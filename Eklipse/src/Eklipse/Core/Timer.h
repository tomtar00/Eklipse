#pragma once

namespace Eklipse
{
	using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

	class EK_API Timer
	{
	public:
		Timer();

		void Record();
		float DeltaTime();

	private:
		inline static Timer* s_instance = nullptr;

		TimePoint m_time;
		TimePoint m_lastTime;
		double m_deltaTime;
	};
}