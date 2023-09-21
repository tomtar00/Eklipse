#pragma once

namespace Eklipse
{
	constexpr int MAX_PROFILED_FRAMES = 100;
	using TimePoint = std::chrono::steady_clock::time_point;

	struct ProfilerNode
	{
		char* name;
		uint32_t signature{ 0 };
		uint32_t threadId{ 0 };
		uint32_t numCalls{ 1 };
		float execTimeMs{ 0.0f };

		std::vector<ProfilerNode> ChildNodes;
	};

	struct ProfilerFrameData
	{
		std::vector<ProfilerNode> ProfileNodes;
	};

	class ProfilerTimer
	{
	public:
		ProfilerTimer() = default;
		ProfilerTimer(char* name);
		~ProfilerTimer();

		void Start(char* name);
		void Stop();
		void AddChildNode(const ProfilerNode& node);
		bool ContainsSignature(uint32_t signature);

		inline float GetTimeMs() { return m_deltaMs; }
		inline char* GetName() { return m_name; }

	private:
		static ProfilerTimer* s_currentTimer;
		ProfilerTimer* m_parentTimer;
		ProfilerNode m_node;

		char* m_name;
		TimePoint m_startTime;
		float m_deltaMs;
	};

	class Profiler
	{
	public:
		static void Init();

		static void Begin(char* name);
		static void End();
		static void EndFrame();

		static std::vector<ProfilerFrameData>& GetData();
		static ProfilerFrameData& GetLastFrameData();

		static ProfilerFrameData FrameData;

	private:
		static std::vector<ProfilerFrameData> m_frameData;
		static ProfilerTimer m_timer;
	};
}

#if defined(__FUNCTION__)
	#define EK_FUNC_SIG __FUNCTION__
#elif defined(__PRETTY_FUNCTION__)
	#define EK_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__func__)
	#define EK_FUNC_SIG __func__
#else
	#define EK_FUNC_SIG "Signature unknown!"
#endif

#ifdef EK_DEBUG
	#define EK_PROFILE_NAME(name)	Eklipse::ProfilerTimer timer(name)
	#define EK_PROFILE()			EK_PROFILE_NAME(EK_FUNC_SIG)

	#define EK_PROFILE_BEGIN(name)	Eklipse::Profiler::Begin(name)
	#define EK_PROFILE_END()		Eklipse::Profiler::End()

	#define EK_PROFILE_END_FRAME()	Eklipse::Profiler::EndFrame()
#else
	#define EK_PROFILE_NAME(name)
	#define EK_PROFILE()	

	#define EK_PROFILE_BEGIN(name)
	#define EK_PROFILE_END()

	#define EK_PROFILE_END_FRAME()
#endif