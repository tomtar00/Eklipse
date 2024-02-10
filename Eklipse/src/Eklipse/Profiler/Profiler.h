#pragma once

#define MAX_PROFILED_FRAMES 100.0f
#define SAMPLE_TIME_INTERVAL 0.5f

namespace Eklipse
{
    struct ProfilerNode
    {
        char* name;
        uint32_t signature{ 0 };
        uint32_t threadId{ 0 };
        uint32_t numCalls{ 1 };
        float execTimeMs{ 0.0f };

        Vec<ProfilerNode> ChildNodes;
    };

    struct ProfilerFrameData
    {
        Vec<ProfilerNode> ProfileNodes;
    };

    class EK_API ProfilerTimer
    {
    public:
        ProfilerTimer() = default;
        ProfilerTimer(char* name);
        ~ProfilerTimer();

        void Start(char* name, bool saveData);
        void Stop();
        void AddChildNode(const ProfilerNode& node);
        ProfilerNode* GetChildNodeBySiganture(uint32_t signature);

        inline float GetTimeMs() const { return m_deltaMs; }
        inline char* GetName() { return m_name; }

    private:
        static ProfilerTimer* s_currentTimer;
        ProfilerTimer* m_parentTimer;
        ProfilerNode m_node;

        char* m_name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
        float m_deltaMs;
        bool m_saveData;
    };

    class EK_API Profiler
    {
    public:
        static void Init();

        static void Begin(char* name);
        static void End();
        static void Submit(float deltaTime);

        static bool IsProfilingCurrentFrame();
        static Vec<ProfilerFrameData>& GetData();
        static ProfilerFrameData& GetLastFrameData();
        static void Clear();

        static ProfilerFrameData FrameData;

    private:
        static Vec<ProfilerFrameData> m_frameData;
        static ProfilerTimer m_timer;
        static float m_timeAcc;
        static bool m_allowProfiling;
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

#define EK_PROFILE_NAME(name)	Eklipse::ProfilerTimer timer(name)
#define EK_PROFILE()			EK_PROFILE_NAME(EK_FUNC_SIG)

#ifdef EK_DEBUG
    #define EK_CORE_PROFILE_NAME(name)	Eklipse::ProfilerTimer timer(name)
    #define EK_CORE_PROFILE()			EK_PROFILE_NAME(EK_FUNC_SIG)

    #define EK_PROFILE_BEGIN(name)	Eklipse::Profiler::Begin(name)
    #define EK_PROFILE_END()		Eklipse::Profiler::End()

    #define EK_PROFILE_END_FRAME()	Eklipse::Profiler::Submit(deltaTime)
#else
    #define EK_CORE_PROFILE_NAME(name)
    #define EK_CORE_PROFILE()	

    #define EK_PROFILE_BEGIN(name)
    #define EK_PROFILE_END()

    #define EK_PROFILE_END_FRAME()
#endif