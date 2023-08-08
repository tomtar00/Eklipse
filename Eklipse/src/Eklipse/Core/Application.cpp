#include "precompiled.h"
#include "Application.h"

#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/Events/KeyEvent.h>
#include <Eklipse/Events/MouseEvent.h>

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/ImGui/ImGuiLayer.h>

namespace Eklipse
{
	ApplicationInfo::ApplicationInfo(const ApplicationInfo& info)
		: appName(info.appName), windowWidth(info.windowWidth),
		windowHeight(info.windowHeight)	{}

	#define CAPTURE_EVENT_FN(x) [this](auto&&... args) -> decltype(auto) { return this->x(args...); }

	Application::Application() : 
		m_running(true), m_minimized(false)
	{
		Init();
	}
	Application::Application(ApplicationInfo& info) : 
		m_running(true) , m_minimized(false),
		m_appInfo(info)
	{
		Init();
	}
	Application::~Application()
	{
		m_scene.Dispose();
	}
	Application& Application::Get()
	{
		return *s_instance;
	}
	ApplicationInfo& Application::GetInfo()
	{
		return m_appInfo;
	}
	Window* Application::GetWindow() const
	{
		return m_window;
	}
	Scene* Application::GetScene()
	{
		return &m_scene;
	}

	void Application::Init()
	{
		s_instance = this;

		WindowData data{ m_appInfo.windowWidth, m_appInfo.windowHeight, m_appInfo.appName };
		m_window = Window::Create(data);
		m_window->SetEventCallback(CAPTURE_EVENT_FN(OnEventReceived));

		m_scene.Load();
		
		m_renderer.SetAPI(ApiType::Vulkan);
		PushLayer(m_debugLayer);
	}

	void Application::OnEventReceived(Event& event)
	{
		EK_CORE_TRACE("{0}", event.ToString());

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(CAPTURE_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(CAPTURE_EVENT_FN(OnWindowResized));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
		{
			if (event.Handled)
				break;

			(*--it)->OnEvent(event);
		}
	}

	void Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_running = false;
	}

	void Application::OnWindowResized(WindowResizeEvent& event)
	{
		
	}

	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
	}

	void Application::Run()
	{
		EK_CORE_INFO("Running engine...");

		float dt = 0;
		while (m_running)
		{
			m_timer.Record();
			dt = m_timer.DeltaTime();

			m_window->Update(dt);

			for (auto& layer : m_layerStack)
			{
				layer->Update(dt);
			}

			m_renderer.Update(dt);

			Stats::Get().Update(dt);
		}

		m_renderer.PostMainLoop();
	}
}