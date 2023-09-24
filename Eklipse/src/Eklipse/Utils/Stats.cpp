#include "precompiled.h"
#include "Stats.h"

namespace Eklipse
{
	const float REFRESH_INTERVAL_SECONDS = 0.5f;

	Stats::Stats() : fps(0), frameTime(0), drawCalls(0), numVertices(0), m_frameAcc(0), m_timeAcc(0)
	{
		frameTimes.resize(30);
	}

	Stats& Stats::Get()
	{
		static Stats stats;
		return stats;
	}
	void Stats::Update(float deltaTime)
	{
		EK_PROFILE_NAME("Stats");

		m_timeAcc += deltaTime;
		m_frameAcc += 1;

		if (m_timeAcc > REFRESH_INTERVAL_SECONDS)
		{
			fps = m_frameAcc / m_timeAcc;

			frameTime = 1 / fps;
			frameTimes.push_back(frameTime);
			frameTimes.erase(frameTimes.begin());

			m_timeAcc = 0.0f;
			m_frameAcc = 0;
		}
	}
	void Stats::Reset()
	{
		drawCalls = 0;
		numVertices = 0;
	}
}
