#include "precompiled.h"

namespace Eklipse
{
	Vec<ProfilerFrameData>	Profiler::m_frameData;
	ProfilerTimer					Profiler::m_timer;
	ProfilerFrameData				Profiler::FrameData;
	float							Profiler::m_timeAcc = 0;
	bool							Profiler::m_allowProfiling = true;

	ProfilerTimer*					ProfilerTimer::s_currentTimer;

	// PROFILER //////////////////////////////////
	void Profiler::Init()
	{
		m_frameData.resize(MAX_PROFILED_FRAMES);
	}
	void Profiler::Begin(char* name)
	{
		m_timer.Start(name, false);
	}
	void Profiler::End()
	{
		m_timer.Stop();
		EK_CORE_TRACE("Stage '{0}' took {1}ms", m_timer.GetName(), m_timer.GetTimeMs());
	}
	void Profiler::Submit(float deltaTime)
	{
		if (m_allowProfiling) m_allowProfiling = false;

		m_timeAcc += deltaTime;
		if (m_timeAcc > SAMPLE_TIME_INTERVAL)
		{
			m_frameData.push_back(FrameData);
			FrameData.ProfileNodes.clear();
			m_frameData.erase(m_frameData.begin());
			m_timeAcc = 0.0f;
			m_allowProfiling = true;
		}
	}
	bool Profiler::IsProfilingCurrentFrame()
	{
		return m_allowProfiling;
	}
	Vec<ProfilerFrameData>& Profiler::GetData()
	{
		return m_frameData;
	}
	ProfilerFrameData& Profiler::GetLastFrameData()
	{
		return m_frameData.back();
	}
	//////////////////////////////////////////////

	// TIMER /////////////////////////////////////
	void AddNode(ProfilerNode& dst, const ProfilerNode& src)
	{
		dst.numCalls += src.numCalls;
		dst.execTimeMs += src.execTimeMs;
		for (size_t i = 0; i < dst.ChildNodes.size(); i++)
		{
			AddNode(dst.ChildNodes[i], src.ChildNodes[i]);
		}
	}
	ProfilerTimer::ProfilerTimer(char* name) : m_name(name), m_parentTimer(nullptr), m_saveData(true)
	{
		if (!Profiler::IsProfilingCurrentFrame()) return;
		Start(name, m_saveData);
	}
	ProfilerTimer::~ProfilerTimer()
	{
		if (!Profiler::IsProfilingCurrentFrame()) return;
		Stop();
	}
	void ProfilerTimer::Start(char* name, bool saveData)
	{
		m_startTime = std::chrono::high_resolution_clock::now();
		m_name = name;
		m_parentTimer = nullptr;

		if (!m_saveData) return;		

		if (s_currentTimer != nullptr && s_currentTimer != this)
		{
			m_parentTimer = s_currentTimer;
		}
		s_currentTimer = this;
	}
	void ProfilerTimer::Stop()
	{
		auto endTime = std::chrono::high_resolution_clock::now();
		m_deltaMs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count() / 1000.0f;

		m_node.name = m_name;
		m_node.signature = std::hash<char*>{}(m_name);
		m_node.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
		m_node.execTimeMs = m_deltaMs;

		if (!m_saveData) return;

		if (m_parentTimer == nullptr)
		{
			Profiler::FrameData.ProfileNodes.push_back(m_node);
		}
		else
		{
			auto signatureNode = m_parentTimer->GetChildNodeBySiganture(m_node.signature);
			if (signatureNode != nullptr)
			{
				AddNode(*signatureNode, m_node);
			}
			else
				m_parentTimer->AddChildNode(m_node);
		}
		s_currentTimer = m_parentTimer;
	}
	void ProfilerTimer::AddChildNode(const ProfilerNode& node)
	{
		m_node.ChildNodes.push_back(node);
	}
	ProfilerNode* ProfilerTimer::GetChildNodeBySiganture(uint32_t signature)
	{
		for (size_t i = 0; i < m_node.ChildNodes.size(); i++)
		{
			if (m_node.ChildNodes[i].signature == signature)
				return &m_node.ChildNodes[i];
		}
		return nullptr;
	}
	//////////////////////////////////////////////
}