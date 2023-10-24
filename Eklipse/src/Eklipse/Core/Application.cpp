#include "precompiled.h"
#include "Application.h"
#include "Input.h"

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Scene/Assets.h>
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/OpenGL/GlImGuiLayer.h>

namespace Eklipse
{
	ApplicationInfo::ApplicationInfo(const ApplicationInfo& info)
		: appName(info.appName), windowWidth(info.windowWidth), windowHeight(info.windowHeight) {}

	Application::Application(ApplicationInfo& info) :
		m_running(true), m_quit(false), m_minimized(false), m_appInfo(info) 
	{
		s_instance = this;
		IMGUI_CHECKVERSION();
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

		WindowData data{ m_appInfo.windowWidth, m_appInfo.windowHeight, m_appInfo.appName };
		m_window = Window::Create(data);
		m_window->SetEventCallback(CAPTURE_FN(OnEventReceived));

		OnInitAPI(Renderer::GetAPI());
		Renderer::Init();
		OnAPIHasInitialized(Renderer::GetAPI());
		m_scene.Load();
		Renderer::InitParameters();

		EK_PROFILE_END();
	}
	void Application::Shutdown()
	{
		EK_PROFILE_BEGIN("Shutdown");

		Renderer::WaitDeviceIdle();

		Assets::Shutdown();

		OnShutdownAPI();
		Renderer::Shutdown();
		OnAPIHasShutdown();

		m_window->Shutdown();
		m_scene.Dispose();
	}

	// === Frame Management ===
	void Application::BeginFrame(float* deltaTime)
	{
		m_timer.Record();
		*deltaTime = m_timer.DeltaTime();
	}
	void Application::EndFrame(float deltaTime)
	{
		m_window->Update(deltaTime);
		Stats::Get().Update(deltaTime);
		EK_PROFILE_END_FRAME(deltaTime);
	}
	void Application::SetAPI(ApiType api)
	{
		EK_CORE_INFO("Setting API to {0}", (int)api);
		Renderer::SetAPI(api);
		m_running = false;
		m_quit = false;
	}

	// === Event Handling ===
	void Application::OnEventReceived(Event& event)
	{
		EK_PROFILE();

		EK_CORE_TRACE(event.ToString());

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(CAPTURE_FN(OnWindowClose));
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
		m_running = false;
		m_quit = true;
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
	void Application::OnMouseMove(MouseMovedEvent& event)
	{
		Input::m_mousePosition = { event.GetX(), event.GetY() };
	}
	void Application::OnMouseScroll(MouseScrolledEvent& event)
	{
		Input::m_mouseScrollDelta = { event.GetXOffset(), event.GetYOffset() };
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
}