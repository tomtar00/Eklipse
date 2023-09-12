#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Timer.h"

#include <Eklipse/Events/Event.h>
#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Scene/Scene.h>

#include <Eklipse/ImGui/ImGuiLayer.h>
#include <Eklipse/ImGui/DebugPanel.h>

namespace Eklipse
{
	struct EK_API ApplicationInfo
	{
		unsigned int windowWidth = 800, windowHeight = 600;
		char* appName = "Eklipse App";
		
		ApplicationInfo() = default;
		ApplicationInfo(const ApplicationInfo& info);
	};

	class EK_API Application
	{
	public:
		Application();
		Application(ApplicationInfo& info);
		~Application();

		static Application& Get();

		ApplicationInfo& GetInfo();
		Ref<Window> GetWindow() const;
		Scene* GetScene();

		void SetAPI(ApiType api);

		void Init();
		void Run();
		void PushLayer(Ref<Layer> layer);
		void SetGuiLayer(const GuiLayerConfigInfo& configInfo);
		void DrawGUI();

		void OnEventReceived(Event& event);
		void OnWindowClose(WindowCloseEvent& event);
		void OnWindowResized(WindowResizeEvent& event);

		Ref<ImGuiLayer> m_guiLayer;

	private:
		inline static Application* s_instance = nullptr;
		ApplicationInfo m_appInfo{};

		Ref<Window> m_window;
		LayerStack m_layerStack;

#ifdef EK_INCLUDE_DEBUG_LAYER
		DebugPanel m_debugPanel;
#endif

		Timer m_timer;
		Scene m_scene;

		bool m_running;
		bool m_minimized;
	};

	Ref<Application> CreateApplication();
}