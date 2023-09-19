#pragma once

namespace Eklipse
{
	using TimePoint = std::chrono::steady_clock::time_point;

	struct ProfileData
	{
		uint32_t threadId;
		float execTimeMs;

		ProfileData() = default;
		ProfileData(const ProfileData& data) : threadId(data.threadId), execTimeMs(data.execTimeMs){}
	};

	class ProfilerTimer
	{
	public:
		ProfilerTimer(const char* name);
		~ProfilerTimer();
	private:
		char* m_name;
		TimePoint m_startTime;
	};

	class Profiler
	{
	public:
		static void AddSample(const char* signature, const ProfileData& sample);
		static std::unordered_map<const char*, Ref<ProfileData>>& GetData();

	private:
		static std::unordered_map<const char*, Ref<ProfileData>> m_profileData;
	};
}

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
	#define EK_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
	#define EK_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
	#define EK_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
	#define EK_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
	#define EK_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
	#define EK_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
	#define EK_FUNC_SIG __func__
#else
	#define EK_FUNC_SIG "Signature unknown!"
#endif

#ifdef EK_DEBUG
	#define EK_PROFILE()	Eklipse::ProfilerTimer timer(EK_FUNC_SIG)
#else
	#define EK_PROFILE()	
#endif