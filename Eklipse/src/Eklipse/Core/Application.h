#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Timer.h"

#include <Eklipse/Events/MouseEvent.h>
#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Scene/Scene.h>

#include <Eklipse/ImGui/ImGuiLayer.h>

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
		Application() = delete;
		Application(ApplicationInfo& info);
		~Application();

		void Init();
		void Shutdown();
		void BeginFrame(float* deltaTime);
		void EndFrame(float deltaTime);
		void SetAPI(ApiType api);

		static Application& Get() { return *s_instance; }
		inline const ApplicationInfo& GetInfo() const { return m_appInfo; }
		inline const Ref<Window> GetWindow() const { return m_window; }
		inline const Scene* GetScene() const { return &m_scene; }
		inline const bool IsRunning() const { return m_running; }
		inline const bool ShouldQuit() const { return m_quit; }
		inline const bool IsMinimized() const { return m_minimized; }

		virtual void OnInitAPI(ApiType api) {}
		virtual void OnAPIHasInitialized(ApiType api) {}
		virtual void OnShutdownAPI() {}
		virtual void OnAPIHasShutdown() {}

		virtual void Run() = 0;

		void PushLayer(Ref<Layer> layer);
		void PushOverlay(Ref<Layer> overlay);
		void PopLayer(Ref<Layer> layer);
		void PopOverlay(Ref<Layer> overlay);

	private:
		void OnEventReceived(Event& event);
		void OnWindowClose(WindowCloseEvent& event);
		void OnWindowResized(WindowResizeEvent& event);
		void OnMouseMove(MouseMovedEvent& event);
		void OnMouseScroll(MouseScrolledEvent& event);

	protected:
		LayerStack m_layerStack;
		Scene m_scene;

	private:
		bool m_running;
		bool m_quit;
		bool m_minimized;
		inline static Application* s_instance = nullptr;
		ApplicationInfo m_appInfo{};
		Ref<Window> m_window;
		Eklipse::MainLoopTimer m_timer;
	};

	Ref<Application> CreateApplication();
}