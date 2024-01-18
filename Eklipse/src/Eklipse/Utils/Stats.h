#pragma once

namespace Eklipse
{
	class EK_API Stats
	{
	public:
		Stats();
		static Stats& Get();

		void Reset();
		void Update(float deltaTime);

		float fps;
		float frameTime;
		std::vector<float> frameTimes;
		uint32_t drawCalls;
		uint32_t numVertices;

	private:
		inline static Stats* s_instance = nullptr;

		float m_timeAcc;
		int m_frameAcc;
	};
}