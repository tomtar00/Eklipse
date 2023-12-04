#pragma once

#include "Core.h"
#include "Timer.h"
#include "Window.h"
#include "LayerStack.h"

#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Renderer/Renderer.h>
#include <Eklipse/Events/MouseEvent.h>
#include <Eklipse/Project/Project.h>
#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/ImGui/DebugPanel.h>

namespace Eklipse
{
	struct ApplicationInfo
	{
		unsigned int windowWidth = 800, windowHeight = 600;
		char* appName = "Eklipse App";
		
		ApplicationInfo() = default;
		ApplicationInfo(const ApplicationInfo& info);
	};

	class Application
	{
	public:
		Application() = delete;
		Application(ApplicationInfo& info);
		virtual ~Application();

		void Init();
		void Shutdown();
		void BeginFrame(float* deltaTime);
		void EndFrame(float deltaTime);
		void SetAPI(ApiType api);

		static Application& Get() { return *s_instance; }
		inline const ApplicationInfo& GetInfo() const { return m_appInfo; }
		inline const Ref<Window> GetWindow() const { return m_window; }
		inline const Ref<AssetLibrary> GetAssetLibrary() const { return m_assetLibrary; }
		inline DebugPanel& GetDebugPanel() { return m_debugPanel; }

		inline const Ref<Scene> GetScene() const { return m_scene; }
		void SwitchScene(Ref<Scene> scene);

		inline const bool IsRunning() const { return m_running; }
		inline const bool ShouldQuit() const { return m_quit; }
		inline const bool IsMinimized() const { return m_minimized; }

		virtual void OnInitAPI(ApiType api) {}
		virtual void OnAPIHasInitialized(ApiType api) {}
		virtual void OnShutdownAPI() {}
		virtual void OnAPIHasShutdown() {}

		void Run();
		void Close();

		virtual void OnPreGUI(float deltaTime) {}
		virtual void OnPostGUI(float deltaTime) {}
		virtual void OnPreUpdate(float deltaTime) {}
		virtual void OnPostUpdate(float deltaTime) {}

		void PushLayer(Ref<Layer> layer);
		void PushOverlay(Ref<Layer> overlay);
		void PopLayer(Ref<Layer> layer);
		void PopOverlay(Ref<Layer> overlay);

		void SubmitToMainThread(const std::function<void()>& function);
		void SubmitToWindowFocus(const std::function<void()>& function);

	private:
		void OnEventReceived(Event& event);
		void OnWindowClose(WindowCloseEvent& event);
		void OnWindowResized(WindowResizeEvent& event);
		void OnWindowFocus(WindowFocusEvent& event);
		void OnMouseMove(MouseMovedEvent& event);
		void OnMouseScroll(MouseScrolledEvent& event);

		void ExecuteMainThreadQueue();

	protected:
		LayerStack m_layerStack;
		Ref<Scene> m_scene;
		Ref<AssetLibrary> m_assetLibrary;
		DebugPanel m_debugPanel;

	private:
		static Application* s_instance;

		bool m_running;
		bool m_quit;
		bool m_minimized;
		ApplicationInfo m_appInfo{};
		Ref<Window> m_window;
		Eklipse::MainLoopTimer m_timer;

		std::vector<std::function<void()>> m_mainThreadQueue;
		std::mutex m_mainThreadQueueMutex;

		std::vector<std::function<void()>> m_mainThreadWindowFocusQueue;
		std::mutex m_mainThreadWindowForcusQueueMutex;
	};

	Ref<Application> CreateApplication();
}