#include "precompiled.h"

namespace Eklipse
{
	std::unordered_map<const char*, Ref<ProfileData>> Profiler::m_profileData;

	void Profiler::AddSample(const char* signature, const ProfileData& sample)
	{
		m_profileData[signature] = CreateRef<ProfileData>(sample);
	}
	std::unordered_map<const char*, Ref<ProfileData>>& Profiler::GetData()
	{
		return m_profileData;
	}

	// TIMER
	ProfilerTimer::ProfilerTimer(const char* name)
	{
		m_startTime = std::chrono::high_resolution_clock::now();
	}
	ProfilerTimer::~ProfilerTimer()
	{
		auto endTime = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count() / 1000000.0f;

		ProfileData profileData{};
		profileData.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
		profileData.execTimeMs = delta;

		Profiler::AddSample(m_name, profileData);
	}
}