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

		SetAPI(ApiType::OpenGL);
		Renderer::Init();
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
		m_appInfo.windowWidth = event.GetWidth();
		m_appInfo.windowHeight = event.GetHeight();
	}

	void Application::PushLayer(Layer* layer)
	{
		if (ImGuiLayer* l = dynamic_cast<ImGuiLayer*>(layer))
		{
			EK_CORE_WARN("Tried to add gui layer as normal layer");
			delete layer;
			return;
		}

		Ref<Layer> layerRef(layer);
		m_layerStack.PushLayer(layerRef);
	}
	void Application::SetGuiLayer(GuiLayerConfigInfo configInfo)
	{
		m_guiLayer = ImGuiLayer::Create(m_window, configInfo);
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

			Stats::Get().Update(dt);

			m_window->Update(dt);

			for (auto& layer : m_layerStack)
			{
				layer->Update(dt);
			}

			Renderer::Update(dt);
		}

		Renderer::Shutdown();
		m_layerStack.Shutdown();
	}
}