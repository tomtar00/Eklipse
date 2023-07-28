#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Timer.h"

#include <Eklipse/Events/Event.h>
#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/Renderer/Renderer.h>

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

			static Application& Get();
			ApplicationInfo& GetInfo();
			Window* GetWindow() const;

			void Init();
			void Run();
			void PushLayer(Layer* layer);

			void OnEventReceived(Event& event);
			void OnWindowClose(WindowCloseEvent& event);
			void OnWindowResized(WindowResizeEvent& event);
	private:
		inline static Application* s_instance = nullptr;
		ApplicationInfo m_appInfo{};

		Window* m_window;
		LayerStack m_layerStack;
		Renderer m_renderer;
		Timer m_timer;

		bool m_running;
		bool m_minimized;
	};

	Application* CreateApplication();
}