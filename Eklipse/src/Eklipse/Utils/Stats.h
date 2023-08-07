#pragma once

namespace Eklipse
{
	class Stats
	{
	public:
		void Update(float deltaTime);

		static Stats& Get();

		float fps;
		float frameTime;
	private:
		inline static Stats* s_instance = nullptr;

		float m_timeAcc;
		int m_frameAcc;
	};
}