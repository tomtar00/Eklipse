#include "precompiled.h"
#include "Application.h"
#include "Input.h"

#include <Eklipse/Scene/SceneManager.h>

namespace Eklipse
{
    ApplicationInfo::ApplicationInfo(const ApplicationInfo& info)
        : appName(info.appName), windowWidth(info.windowWidth), windowHeight(info.windowHeight) {}

    // === Static Initialization ===
    Application* Application::s_instance = nullptr;
    Application::Application(ApplicationInfo& info) :
        m_running(true), m_quit(false), m_minimized(false), m_appInfo(info) 
    {
        EK_ASSERT(s_instance == nullptr, "Application already exists!");
        s_instance = this;
    }
    Application::~Application()
    {
        m_layerStack.Shutdown();
        EK_PROFILE_END();
    }

    // === Initialization and Shutdown ===
    void Application::Init()
    {
        EK_PROFILE_BEGIN("Startup");

        m_running = true;
        m_quit = false;

        int tries = 0;
        ApiType api = Renderer::GetAPI();
        do
        {
            Renderer::SetAPI((ApiType)(((int)api + tries) % API_TYPE_COUNT));

            if (++tries > API_TYPE_COUNT)
            {
                EK_CORE_CRITICAL("Could not find any supported Graphics API!");
                exit(-1);
            }

            WindowData data{ m_appInfo.windowWidth, m_appInfo.windowHeight, m_appInfo.appName };
            m_window.reset();
            m_window = Window::Create(data);
            m_window->SetEventCallback(CAPTURE_FN(OnEventReceived));

            OnInitAPI(Renderer::GetAPI());
        } 
        while (!Renderer::Init());

        OnAPIHasInitialized(Renderer::GetAPI());
        for (auto& layer : m_layerStack)
        {
            layer->OnAPIHasInitialized(Renderer::GetAPI());
        }
        Renderer::InitParameters();

        EK_PROFILE_END();
    }
    void Application::Shutdown()
    {
        EK_PROFILE_BEGIN("Shutdown");

        Renderer::WaitDeviceIdle();

        for (auto& layer : m_layerStack)
        {
            layer->OnShutdownAPI(m_quit);
        }
        OnShutdownAPI(m_quit);
        Renderer::Shutdown();
        OnAPIHasShutdown();

        if (m_quit)
        {
            SceneManager::Dispose();
        }
    }

    // === Frame Management ===
    void Application::BeginFrame(float* deltaTime)
    {
        EK_PROFILE_NAME("BeginFrame");
        m_timer.Record();
        *deltaTime = m_timer.DeltaTime();

        ExecuteMainThreadQueue();
    }
    void Application::EndFrame(float deltaTime)
    {
        {
            EK_PROFILE_NAME("EndFrame");
            Input::Reset();
            m_window->Update(deltaTime);
            Stats::Get().Update(deltaTime);
        }
        EK_PROFILE_END_FRAME(deltaTime);
    }

    // === Getters ===
    Application& Application::Get()									{ return *s_instance; }
    const ApplicationInfo& Application::GetInfo() const				{ return m_appInfo; }
    const Ref<Window> Application::GetWindow() const				{ return m_window; }
    const bool Application::IsRunning() const						{ return m_running; }
    const bool Application::ShouldQuit() const						{ return m_quit; }
    const bool Application::IsMinimized() const						{ return m_minimized; }

    // === Setters ===
    void Application::SetAPI(ApiType api)
    {
        EK_CORE_PROFILE();
        if (Renderer::GetAPI() == api)
        {
            EK_CORE_WARN("API already set to {0}", APITypeToString(api));
            return;
        }
        EK_CORE_INFO("Setting API to {0}", APITypeToString(api));

        Renderer::SetAPI(api);

        m_running = false;
        m_quit = false;
    }

    // === Event Handling ===
    void Application::OnEventReceived(Event& event)
    {
        EK_CORE_PROFILE();

        //EK_CORE_TRACE(event.ToString());

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<WindowCloseEvent>(CAPTURE_FN(OnWindowClose), true);
        dispatcher.Dispatch<WindowFocusEvent>(CAPTURE_FN(OnWindowFocus), true);
        dispatcher.Dispatch<WindowResizeEvent>(CAPTURE_FN(OnWindowResized), false);
        dispatcher.Dispatch<MouseMovedEvent>(CAPTURE_FN(OnMouseMove), true);
        dispatcher.Dispatch<MouseScrolledEvent>(CAPTURE_FN(OnMouseScroll), true);

        for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
        {
            if (event.Handled)
                break;

            (*--it)->OnEvent(event);
        }
    }
    void Application::OnWindowClose(WindowCloseEvent& event)
    {
        EK_CORE_PROFILE();
        Close();
    }
    void Application::OnWindowResized(WindowResizeEvent& event)
    {
        EK_CORE_PROFILE();
        m_appInfo.windowWidth = event.GetWidth();
        m_appInfo.windowHeight = event.GetHeight();

        if (m_appInfo.windowWidth == 0 || m_appInfo.windowHeight == 0)
        {
            m_minimized = true;
            m_window->WaitEvents();
            return;
        }

        m_minimized = false;
        Renderer::OnWindowResize(m_appInfo.windowWidth, m_appInfo.windowHeight);
    }
    void Application::OnWindowFocus(WindowFocusEvent& event)
    {
        EK_CORE_PROFILE();
        std::scoped_lock<std::mutex> lock(m_mainThreadWindowForcusQueueMutex);

        for (auto& func : m_mainThreadWindowFocusQueue)
            func();

        m_mainThreadWindowFocusQueue.clear();
    }
    void Application::OnMouseMove(MouseMovedEvent& event)
    {
        EK_CORE_PROFILE();
        Input::m_mousePosition = { event.GetX(), event.GetY() };
    }
    void Application::OnMouseScroll(MouseScrolledEvent& event)
    {
        EK_CORE_PROFILE();
        Input::m_lastMousePosition = Input::m_mousePosition;
        Input::m_mouseScrollDelta = { event.GetXOffset(), event.GetYOffset() };
    }

    // === Main Loop ===
    void Application::Run()
    {
        EK_CORE_TRACE("========== Starting Eklipse Engine ==========");

        Application::Init();

        float deltaTime = 0.0f;
        while (Application::IsRunning())
        {
            Application::BeginFrame(&deltaTime);

            if (!Application::IsMinimized())
            {
                {
                    EK_PROFILE_NAME("GUI");

                    OnPreGUI(deltaTime);
                    for (auto& layer : m_layerStack)
                    {
                        layer->OnGUI(deltaTime);
                    }
                    OnPostGUI(deltaTime);
                }

                {
                    EK_PROFILE_NAME("Update");

                    Renderer::BeginFrame();
                    for (auto& layer : m_layerStack)
                    {
                        layer->OnUpdate(deltaTime);
                    }
                    Renderer::BeginDefaultRenderPass();
                    for (auto& layer : m_layerStack)
                    {
                        layer->OnRender();
                    }
                    Renderer::EndDefaultRenderPass();
                    Renderer::Submit();
                }
            }

            Application::EndFrame(deltaTime);
        }

        EK_CORE_TRACE("========== Closing Eklipse Engine ==========");

        Application::Shutdown();
    }
    void Application::Close()
    {
        m_running = false;
        m_quit = true;
    }

    // === Layer Management ===
    void Application::PushLayer(Ref<Layer> layer)
    {
        EK_CORE_PROFILE();
        m_layerStack.PushLayer(layer);
    }
    void Application::PushOverlay(Ref<Layer> overlay)
    {
        EK_CORE_PROFILE();
        m_layerStack.PushOverlay(overlay);
    }
    void Application::PopLayer(Ref<Layer> layer)
    {
        EK_CORE_PROFILE();
        m_layerStack.PopLayer(layer);
    }
    void Application::PopOverlay(Ref<Layer> overlay)
    {
        EK_CORE_PROFILE();
        m_layerStack.PopOverlay(overlay);
    }

    // === Fucntion Queues ===
    void Application::SubmitToMainThread(const std::function<void()>& function)
    {
        EK_CORE_PROFILE();
        std::scoped_lock<std::mutex> lock(m_mainThreadQueueMutex);

        m_mainThreadQueue.emplace_back(function);
    }
    void Application::ExecuteMainThreadQueue()
    {
        std::scoped_lock<std::mutex> lock(m_mainThreadQueueMutex);

        for (auto& func : m_mainThreadQueue)
            func();

        m_mainThreadQueue.clear();
    }
    void Application::SubmitToWindowFocus(const std::function<void()>& function)
    {
        EK_CORE_PROFILE();
        m_mainThreadWindowFocusQueue.emplace_back(function);
    }
}