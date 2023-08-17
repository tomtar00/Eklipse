#include "precompiled.h"
#include "Application.h"

#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/Events/KeyEvent.h>
#include <Eklipse/Events/MouseEvent.h>

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/ImGui/ImGuiLayer.h>
#include <Eklipse/Platform/Vulkan/VkImGuiLayer.h>

namespace Eklipse
{
	ApplicationInfo::ApplicationInfo(const ApplicationInfo& info)
		: appName(info.appName), windowWidth(info.windowWidth),
		windowHeight(info.windowHeight)	{}

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

	void Application::SetAPI(ApiType api)
	{
		m_renderer.SetAPI(api, 
		[this]()
		{
			if (m_guiLayer != nullptr)
				m_guiLayer->Shutdown();
		},
		[this]()
		{
			if (m_guiLayer != nullptr)
				m_guiLayer->Init();
		});
	}

	void Application::Init()
	{
		s_instance = this;

		WindowData data{ m_appInfo.windowWidth, m_appInfo.windowHeight, m_appInfo.appName };
		m_window = Window::Create(data);
		m_window->SetEventCallback(CAPTURE_FN(OnEventReceived));

		m_scene.Load();

		m_guiLayer = nullptr;
		IMGUI_CHECKVERSION();
		SetAPI(ApiType::Vulkan);
	}

	void Application::OnEventReceived(Event& event)
	{
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
		
	}

	void Application::PushLayer(Layer* layer)
	{
		if (ImGuiLayer* l = dynamic_cast<ImGuiLayer*>(layer))
		{
			EK_CORE_WARN("Tried to add gui layer as normal layer");
			delete layer;
			return;
		}

		m_layerStack.PushLayer(layer);
	}
	void Application::SetGuiLayer(GuiLayerConfigInfo configInfo)
	{
		delete m_guiLayer;
		switch (m_renderer.GetAPI())
		{
			case ApiType::Vulkan:
			{
				m_guiLayer = new Vulkan::VkImGuiLayer(m_window, configInfo);
				break;
			}
		}
		EK_ASSERT(m_guiLayer, "GUI API {0} not implemented!", (int)m_renderer.GetAPI());
		m_layerStack.PushLayer(m_guiLayer);
		m_guiLayer->Init();
	}

	void Application::Run()
	{
		EK_CORE_INFO("Running engine...");

//#ifndef EK_EDITOR
//		bool enable = true;
//		GuiLayerConfigInfo debugLayerCreateInfo{};
//		debugLayerCreateInfo.enabled = &enable;
//		debugLayerCreateInfo.menuBarEnabled = false;
//		debugLayerCreateInfo.dockingEnabled = false;
//		ImGuiLayer::s_ctx = ImGui::CreateContext();
//		SetGuiLayer(debugLayerCreateInfo);
//#endif

#ifdef EK_INCLUDE_DEBUG_LAYER
		m_guiLayer->AddPanel(m_debugPanel);
#endif

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

		m_guiLayer->Shutdown();
	}
}