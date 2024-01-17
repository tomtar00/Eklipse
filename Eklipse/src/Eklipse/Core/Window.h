#pragma once

#include "Eklipse/Events/Event.h"
#include <GLFW/glfw3.h>

namespace Eklipse
{
	struct WindowData
	{
		int width{ 0 }, height{ 0 };
		char* title{ "" };

		std::function<void(Event&)> EventCallback;
	};

	class Window
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

		// platform dependent
		inline virtual GLFWwindow* GetGlfwWindow() { return nullptr; };

	protected:
		WindowData m_data;
	};
}
