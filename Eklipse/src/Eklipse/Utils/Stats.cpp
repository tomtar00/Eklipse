#include "precompiled.h"
#include "Stats.h"

namespace Eklipse
{
	const float FPS_REFRESH_INTERVAL_SECONDS = 0.5f;

	void Stats::Update(float deltaTime)
	{
		frameTime = deltaTime;

		m_timeAcc += deltaTime;
		m_frameAcc += 1;

		if (m_timeAcc > FPS_REFRESH_INTERVAL_SECONDS)
		{
			fps = m_frameAcc / m_timeAcc;

			m_timeAcc = 0.0f;
			m_frameAcc = 0;
		}
	}
	Stats& Stats::Get()
	{
		static Stats stats;
		return stats;
	}
}
