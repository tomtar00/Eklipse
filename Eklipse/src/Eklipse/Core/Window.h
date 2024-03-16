#pragma once

#include <GLFW/glfw3.h>
#include <Eklipse/Events/Event.h>
#include <Eklipse/Renderer/GraphicsAPI.h>

namespace Eklipse
{
	struct WindowData
	{
		int width{ 0 }, height{ 0 };
		char* title{ "" };
		GraphicsAPI::Type clientAPI;

		std::function<void(Event&)> EventCallback;
	};
	enum class CursorMode
    {
        Normal,
        Hidden,
        Disabled
    };

	class EK_API Window
	{
	public:
		static Ref<Window> Create(WindowData& data);

		Window(WindowData& data) : m_data(data) {}
		virtual ~Window() = default;
		virtual void Shutdown() = 0;

		void GetFramebufferSize(int& width, int& height);
		inline void SetEventCallback(const std::function<void(Event&)>& callback) { m_data.EventCallback = callback; }
		inline WindowData& GetData() { return m_data; }

		virtual void Update(float deltaTime) {}
		virtual void SetWindowHint(int hint, int value) = 0;
		virtual void SwapBuffers() = 0;
		virtual void WaitEvents() = 0;
		virtual void SetTitle(const char* title) = 0;
		virtual void Maximize() = 0;
		virtual bool IsMaximized() = 0;
		virtual void SetCursorMode(CursorMode mode) = 0;

		// platform dependent
		inline virtual GLFWwindow* GetGlfwWindow() { return nullptr; };

	protected:
		WindowData m_data;
	};
}
