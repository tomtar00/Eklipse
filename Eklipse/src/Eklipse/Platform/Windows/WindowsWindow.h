#pragma once

#define GLFW_INCLUDE_NONE

#include <Eklipse/Core/Window.h>
#include <GLFW/glfw3.h>

namespace Eklipse
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(WindowData& spec);
		virtual ~WindowsWindow();

		GLFWwindow* GetGlfwWindow();

		void Update(float deltaTime) override;

	private:
		void Init();
		void Shutdown();

		GLFWwindow* m_window;
	};
}