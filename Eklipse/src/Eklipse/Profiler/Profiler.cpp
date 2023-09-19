#include "precompiled.h"

namespace Eklipse
{
	std::unordered_map<const char*, ProfileNode> Profiler::m_profileData;
	ProfilerTimer* ProfilerTimer::s_currentTimer;

	void Profiler::AddSample(const char* signature, const ProfileNode& sample)
	{
		m_profileData[signature] = sample;
	}
	std::unordered_map<const char*, ProfileNode>& Profiler::GetData()
	{
		return m_profileData;
	}

	// TIMER
	ProfilerTimer::ProfilerTimer(char* name) : m_name(name), m_parentTimer(nullptr)
	{
		if (s_currentTimer != nullptr)
		{
			m_parentTimer = s_currentTimer;
		}
		s_currentTimer = this;

		m_startTime = std::chrono::high_resolution_clock::now();
	}
	ProfilerTimer::~ProfilerTimer()
	{
		auto endTime = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count() / 1000.0f;

		m_node.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
		m_node.execTimeMs = delta;

		if (m_parentTimer == nullptr)
		{
			Profiler::AddSample(m_name, m_node);
		}
		else
		{
			// TODO: add to parent data node
			m_parentTimer->AddChildNode(m_name, m_node);
		}
		s_currentTimer = m_parentTimer;
	}
	void ProfilerTimer::AddChildNode(const char* signature, const ProfileNode& data)
	{
		m_node.childNodes[signature] = data;
	}
}