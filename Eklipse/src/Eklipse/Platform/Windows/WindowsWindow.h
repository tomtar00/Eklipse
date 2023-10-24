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
		virtual void Shutdown() override;

		virtual GLFWwindow* GetGlfwWindow() override;
		virtual void Update(float deltaTime) override;

		virtual void SetWindowHint(int attrib, int value) override;
		virtual void SwapBuffers() override;
		virtual void WaitEvents() override;

	private:
		inline static bool s_glfwInitialized = false;
		GLFWwindow* m_window;
	};
}