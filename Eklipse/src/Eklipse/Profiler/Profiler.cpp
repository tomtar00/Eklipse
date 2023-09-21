#include "precompiled.h"

namespace Eklipse
{
	std::vector<ProfilerFrameData>	Profiler::m_frameData;
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
		m_allowProfiling = true;
		m_timer.Start(name);
	}
	void Profiler::End()
	{
		m_timer.Stop();
		EK_CORE_TRACE("Stage '{0}' took {1}ms", m_timer.GetName(), m_timer.GetTimeMs());
	}
	void Profiler::EndFrame(float deltaTime)
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
	bool Profiler::CanProfile()
	{
		return m_allowProfiling;
	}
	std::vector<ProfilerFrameData>& Profiler::GetData()
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
	ProfilerTimer::ProfilerTimer(char* name) : m_name(name), m_parentTimer(nullptr)
	{
		Start(name);
	}
	ProfilerTimer::~ProfilerTimer()
	{
		Stop();
	}
	void ProfilerTimer::Start(char* name)
	{
		if (!Profiler::CanProfile()) return;

		m_name = name;
		m_parentTimer = nullptr;

		if (s_currentTimer != nullptr && s_currentTimer != this)
		{
			m_parentTimer = s_currentTimer;
		}
		s_currentTimer = this;

		m_startTime = std::chrono::high_resolution_clock::now();
	}
	void ProfilerTimer::Stop()
	{
		if (!Profiler::CanProfile()) return;

		auto endTime = std::chrono::high_resolution_clock::now();
		m_deltaMs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count() / 1000.0f;

		m_node.name = m_name;
		m_node.signature = std::hash<char*>{}(m_name);
		m_node.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
		m_node.execTimeMs = m_deltaMs;

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