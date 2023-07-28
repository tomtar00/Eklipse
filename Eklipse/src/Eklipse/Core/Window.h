#pragma once

#include "Eklipse/Events/Event.h"

namespace Eklipse
{
	struct WindowData
	{
		int width = 0, height = 0;
		char* title = "";
		bool minimized;
		bool framebufferResized;

		std::function<void(Event&)> EventCallback;
	};

	class Window
	{
	public:
		virtual ~Window() {}

		WindowData& GetData() { return m_data; }

		void SetEventCallback(const std::function<void(Event&)>& callback) { m_data.EventCallback = callback; }

		virtual void Update() {}
		virtual void GetFramebufferSize(int& width, int& height) {}

		static Window* Create(WindowData& data);

	protected:
		WindowData m_data;
		inline static bool s_glfwInitialized = false;
	};
}
