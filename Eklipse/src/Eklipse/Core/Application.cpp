#include "precompiled.h"
#include "Application.h"

#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/Events/KeyEvent.h>
#include <Eklipse/Events/MouseEvent.h>

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>
#include <Eklipse/Platform/OpenGL/GlImGuiLayer.h>

namespace Eklipse
{
	ApplicationInfo::ApplicationInfo(const ApplicationInfo& info)
		: appName(info.appName), windowWidth(info.windowWidth),
		windowHeight(info.windowHeight) {}

	Application::Application() :
		m_running(true), m_minimized(false)
	{
		Init();
	}
	Application::Application(ApplicationInfo& info) :
		m_running(true), m_minimized(false),
		m_appInfo(info)
	{
		Init();
	}
	Application::~Application()
	{
		m_scene.Dispose();
		EK_PROFILE_END();
	}
	Application& Application::Get()
	{
		return *s_instance;
	}
	ApplicationInfo& Application::GetInfo()
	{
		return m_appInfo;
	}
	Ref<Window> Application::GetWindow() const
	{
		return m_window;
	}
	Scene* Application::GetScene()
	{
		return &m_scene;
	}

	void Application::SetAPI(ApiType api)
	{
		Renderer::SetAPI(api,
			[this]()
			{
				if (m_guiLayer != nullptr)
					m_guiLayer->Shutdown();
			},
			[this]()
			{
				if (m_guiLayer != nullptr)
					m_guiLayer->Init();
			}
		);
	}

	void Application::Init()
	{
		s_instance = this;

		Renderer::SetStartupAPI(ApiType::OpenGL);

		WindowData data{ m_appInfo.windowWidth, m_appInfo.windowHeight, m_appInfo.appName };
		m_window = Window::Create(data);
		m_window->SetEventCallback(CAPTURE_FN(OnEventReceived));

		m_guiLayer = nullptr;
		IMGUI_CHECKVERSION();

		SetAPI(Renderer::GetAPI());
		Renderer::Init();

		m_scene.Load();
	}

	void Application::OnEventReceived(Event& event)
	{
		EK_PROFILE();

		EK_CORE_TRACE(event.ToString());

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(CAPTURE_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(CAPTURE_FN(OnWindowResized));

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

	void Application::PushLayer(Ref<Layer> layer)
	{
		m_layerStack.PushLayer(layer);
	}
	void Application::PushOverlay(Ref<ImGuiLayer> overlay)
	{
		m_layerStack.PushOverlay(overlay);
		m_guiLayer = overlay;
		m_guiLayer->Init();
	}

	void Application::DrawGUI()
	{
		EK_PROFILE();

		m_guiLayer->Draw();
	}

	void Application::Run()
	{
		EK_CORE_INFO("========== Starting Eklipse Engine ==========");

		/*#ifndef EK_EDITOR
				bool enable = true;
				GuiLayerConfigInfo debugLayerCreateInfo{};
				debugLayerCreateInfo.enabled = &enable;
				debugLayerCreateInfo.menuBarEnabled = false;
				debugLayerCreateInfo.dockingEnabled = false;
				ImGuiLayer::s_ctx = ImGui::CreateContext();
				SetGuiLayer(debugLayerCreateInfo);
		#endif*/

//#ifdef EK_INCLUDE_DEBUG_LAYER
//		m_guiLayer->AddPanel(m_debugPanel);
//#endif

		m_running = true;

		float dt = 0;
		while (m_running)
		{
			m_timer.Record();
			dt = m_timer.DeltaTime();

			for (auto& layer : m_layerStack)
			{
				layer->OnUpdate(dt);
			}

			for (auto& layer : m_layerStack)
			{
				layer->OnGUI();
			}

			Stats::Get().Reset();
			Renderer::Update(dt);
			Stats::Get().Update(dt);

			m_window->Update(dt);

			EK_PROFILE_END_FRAME(dt);
		}

		EK_CORE_INFO("========== Closing Eklipse Engine ==========");

		EK_PROFILE_BEGIN("Shutdown");

		Renderer::Shutdown();
		m_window->Shutdown();
		m_layerStack.Shutdown();
	}
}