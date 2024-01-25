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
#include <Eklipse/ImGui/Terminal/TerminalPanel.h>
#include <Eklipse/Scripting/ScriptLinker.h>

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
		virtual ~Application();

		// === Initialization and Shutdown ===
		void Init();
		void Shutdown();

		// === Frame Management ===
		void BeginFrame(float* deltaTime);
		void EndFrame(float deltaTime);

		// === Getters ===
		static Application& Get();
		const ApplicationInfo& GetInfo() const;
		const Ref<Window> GetWindow() const;
		const bool IsRunning() const;
		const bool ShouldQuit() const;
		const bool IsMinimized() const;

		// === Setters ===
		void SetAPI(ApiType api);

		// === Virtual Event Handling ===
		virtual void OnInitAPI(ApiType api) {}
		virtual void OnAPIHasInitialized(ApiType api) {}
		virtual void OnShutdownAPI() {}
		virtual void OnAPIHasShutdown() {}
		virtual void OnPreGUI(float deltaTime) {}
		virtual void OnPostGUI(float deltaTime) {}

		// === Main Loop ===
		void Run();
		void Close();

		// === Layer Management ===
		void PushLayer(Ref<Layer> layer);
		void PushOverlay(Ref<Layer> overlay);
		void PopLayer(Ref<Layer> layer);
		void PopOverlay(Ref<Layer> overlay);

		// === Fucntion Queues ===
		void SubmitToMainThread(const std::function<void()>& function);
		void SubmitToWindowFocus(const std::function<void()>& function);

	private:

		// === Event Handling ===
		void OnEventReceived(Event& event);
		void OnWindowClose(WindowCloseEvent& event);
		void OnWindowResized(WindowResizeEvent& event);
		void OnWindowFocus(WindowFocusEvent& event);
		void OnMouseMove(MouseMovedEvent& event);
		void OnMouseScroll(MouseScrolledEvent& event);

		void ExecuteMainThreadQueue();

	protected:
		Ref<Window> m_window;
		LayerStack m_layerStack;

	private:
		static Application* s_instance;
		ApplicationInfo m_appInfo{};

		bool m_running;
		bool m_quit;
		bool m_minimized;

		MainLoopTimer m_timer;

		std::vector<std::function<void()>> m_mainThreadQueue;
		std::mutex m_mainThreadQueueMutex;

		std::vector<std::function<void()>> m_mainThreadWindowFocusQueue;
		std::mutex m_mainThreadWindowForcusQueueMutex;
	};

	Ref<Application> CreateApplication();
}