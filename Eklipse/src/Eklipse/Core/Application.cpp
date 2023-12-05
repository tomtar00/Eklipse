#include "precompiled.h"
#include "Application.h"
#include "Input.h"

namespace Eklipse
{
	Application* Application::s_instance = nullptr;

	ApplicationInfo::ApplicationInfo(const ApplicationInfo& info)
		: appName(info.appName), windowWidth(info.windowWidth), windowHeight(info.windowHeight) {}

	Application::Application(ApplicationInfo& info) :
		m_running(true), m_quit(false), m_minimized(false), m_appInfo(info) 
	{
		EK_ASSERT(s_instance == nullptr, "Application already exists!");
		s_instance = this;

		m_scene = CreateRef<Scene>();
		m_assetLibrary = CreateRef<AssetLibrary>();
	}
	Application::~Application()
	{
		m_scene->Unload();
		m_layerStack.Shutdown();
		EK_PROFILE_END();
	}

	// === Initialization and Shutdown ===
	void Application::Init()
	{
		EK_PROFILE_BEGIN("Startup");

		m_running = true;
		m_quit = false;

		// Init window
		WindowData data{ m_appInfo.windowWidth, m_appInfo.windowHeight, m_appInfo.appName };
		m_window = Window::Create(data);
		m_window->SetEventCallback(CAPTURE_FN(OnEventReceived));

		// Init API
		OnInitAPI(Renderer::GetAPI());
		Renderer::Init();
		OnAPIHasInitialized(Renderer::GetAPI());
		
		// Init assets
		if (Project::GetActive())
			Project::GetActive()->LoadAssets();
		m_assetLibrary->Load("Assets");

		Renderer::InitParameters();

		// Apply all components in the active scene
		m_scene->ApplyAllComponents();

		EK_PROFILE_END();
	}
	void Application::Shutdown()
	{
		EK_PROFILE_BEGIN("Shutdown");

		Renderer::WaitDeviceIdle();

		if (Project::GetActive())
			Project::GetActive()->UnloadAssets();
		m_assetLibrary->Unload();

		OnShutdownAPI();
		Renderer::Shutdown();
		OnAPIHasShutdown();

		m_window->Shutdown();
	}

	// === Frame Management ===
	void Application::BeginFrame(float* deltaTime)
	{
		m_timer.Record();
		*deltaTime = m_timer.DeltaTime();

		ExecuteMainThreadQueue();
	}
	void Application::EndFrame(float deltaTime)
	{
		m_window->Update(deltaTime);
		Stats::Get().Update(deltaTime);
		EK_PROFILE_END_FRAME(deltaTime);
	}
	void Application::SetAPI(ApiType api)
	{
		if (Renderer::GetAPI() == api)
		{
			EK_CORE_WARN("API already set to {0}", (int)api);
			return;
		}
		EK_CORE_INFO("Setting API to {0}", (int)api);
		Renderer::SetAPI(api);

		m_running = false;
		m_quit = false;
	}

	// === Scene Management ===
	void Application::SwitchScene(Ref<Scene> scene)
	{
		m_scene->Unload();
		m_scene.reset();
		m_scene = scene;
	}

	// === Event Handling ===
	void Application::OnEventReceived(Event& event)
	{
		EK_PROFILE();

		EK_CORE_TRACE(event.ToString());

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(CAPTURE_FN(OnWindowClose));
		dispatcher.Dispatch<WindowFocusEvent>(CAPTURE_FN(OnWindowFocus));
		dispatcher.Dispatch<WindowResizeEvent>(CAPTURE_FN(OnWindowResized));
		dispatcher.Dispatch<MouseMovedEvent>(CAPTURE_FN(OnMouseMove));
		dispatcher.Dispatch<MouseScrolledEvent>(CAPTURE_FN(OnMouseScroll));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
		{
			if (event.Handled)
				break;

			(*--it)->OnEvent(event);
		}
	}
	void Application::OnWindowClose(WindowCloseEvent& event)
	{
		Close();
	}
	void Application::OnWindowResized(WindowResizeEvent& event)
	{
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
		std::scoped_lock<std::mutex> lock(m_mainThreadWindowForcusQueueMutex);

		for (auto& func : m_mainThreadWindowFocusQueue)
			func();

		m_mainThreadWindowFocusQueue.clear();
	}
	void Application::OnMouseMove(MouseMovedEvent& event)
	{
		Input::m_mousePosition = { event.GetX(), event.GetY() };
	}
	void Application::OnMouseScroll(MouseScrolledEvent& event)
	{
		Input::m_mouseScrollDelta = { event.GetXOffset(), event.GetYOffset() };
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_mainThreadQueueMutex);

		for (auto& func : m_mainThreadQueue)
			func();

		m_mainThreadQueue.clear();
	}

	void Application::Run()
	{
		EK_INFO("========== Starting Eklipse Editor ==========");

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

					OnPreUpdate(deltaTime);
					for (auto& layer : m_layerStack)
					{
						layer->OnUpdate(deltaTime);
					}
					OnPostUpdate(deltaTime);
				}
			}

			Application::EndFrame(deltaTime);
		}

		EK_INFO("========== Closing Eklipse Editor ==========");

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
		m_layerStack.PushLayer(layer);
	}
	void Application::PushOverlay(Ref<Layer> overlay)
	{
		m_layerStack.PushOverlay(overlay);
	}
	void Application::PopLayer(Ref<Layer> layer)
	{
		m_layerStack.PopLayer(layer);
	}
	void Application::PopOverlay(Ref<Layer> overlay)
	{
		m_layerStack.PopOverlay(overlay);
	}
	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_mainThreadQueueMutex);

		m_mainThreadQueue.emplace_back(function);
	}
	void Application::SubmitToWindowFocus(const std::function<void()>& function)
	{
		m_mainThreadWindowFocusQueue.emplace_back(function);
	}
}