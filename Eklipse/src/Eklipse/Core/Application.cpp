#include "precompiled.h"
#include "Application.h"
#include "Input.h"

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/OpenGL/GlImGuiLayer.h>

namespace Eklipse
{
	ApplicationInfo::ApplicationInfo(const ApplicationInfo& info)
		: appName(info.appName), windowWidth(info.windowWidth),
		windowHeight(info.windowHeight) {}

	Application::Application(ApplicationInfo& info) :
		m_running(true), m_minimized(false),
		m_appInfo(info) 
	{
		s_instance = this;

		Renderer::SetStartupAPI(ApiType::Vulkan);

		WindowData data{ m_appInfo.windowWidth, m_appInfo.windowHeight, m_appInfo.appName };
		m_window = Window::Create(data);
		m_window->SetEventCallback(CAPTURE_FN(OnEventReceived));

		IMGUI_CHECKVERSION();
	}
	Application::~Application()
	{
		EK_PROFILE_END();
	}
	void Application::Init()
	{
		SetAPI(Renderer::GetAPI());
		m_scene.Load();
		Renderer::Init();

		EK_PROFILE_END();
	}
	void Application::Shutdown()
	{
		EK_PROFILE_BEGIN("Shutdown");

		m_layerStack.Shutdown();
		Renderer::Shutdown();
		m_window->Shutdown();
		m_scene.Dispose();
	}
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
	}
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
	}
	void Application::OnWindowResized(WindowResizeEvent& event)
	{
		m_appInfo.windowWidth = event.GetWidth();
		m_appInfo.windowHeight = event.GetHeight();

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
	void Application::PushLayer(Ref<Layer> layer)
	{
		m_layerStack.PushLayer(layer);
	}
	void Application::PushOverlay(Ref<Layer> overlay)
	{
		m_layerStack.PushOverlay(overlay);
	}
}