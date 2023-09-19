#pragma once

namespace Eklipse
{
	using TimePoint = std::chrono::steady_clock::time_point;

	struct ProfileNode
	{
		uint32_t threadId;
		float execTimeMs;

		std::unordered_map<const char*, ProfileNode> childNodes;

		ProfileNode() = default;
		ProfileNode(const ProfileNode& data) : threadId(data.threadId), execTimeMs(data.execTimeMs){}
	};

	class ProfilerTimer
	{
	public:
		ProfilerTimer(char* name);
		~ProfilerTimer();
		void AddChildNode(const char* signature, const ProfileNode& data);

	private:
		static ProfilerTimer* s_currentTimer;
		ProfilerTimer* m_parentTimer;
		ProfileNode m_node;

		char* m_name;
		TimePoint m_startTime;
	};

	class Profiler
	{
	public:
		static void AddSample(const char* signature, const ProfileNode& sample);
		static std::unordered_map<const char*, ProfileNode>& GetData();

	private:
		static std::unordered_map<const char*, ProfileNode> m_profileData;
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
	#define EK_PROFILE()	Eklipse::ProfilerTimer timer(EK_FUNC_SIG)
#else
	#define EK_PROFILE()	
#endif