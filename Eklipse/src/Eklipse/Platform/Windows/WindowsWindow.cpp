#include "precompiled.h"
#include "WindowsWindow.h"

#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/Events/KeyEvent.h>
#include <Eklipse/Events/MouseEvent.h>

#include <Eklipse/Utils/Stats.h>
#include <Eklipse/Renderer/Renderer.h>

#define GLFW_INCLUDE_VULKAN
#define PROPAGATE_GLFW_EVENT(x) ((WindowData*)glfwGetWindowUserPointer(window))->EventCallback(x)

namespace Eklipse
{
	void GlfwErrorCallback(int error, const char* description)
	{
		EK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}
	void GlfwWindowSizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowResizeEvent event(width, height);
		PROPAGATE_GLFW_EVENT(event);
	}
	/*void GlfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		FramebufferResizeEvent event(width, height);

		((WindowData*)glfwGetWindowUserPointer(window))->framebufferResized = true;
		PROPAGATE_GLFW_EVENT(event);
	}*/
	void GlfwWindowCloseCallback(GLFWwindow* window)
	{
		WindowCloseEvent event;
		PROPAGATE_GLFW_EVENT(event);
	}
	void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		switch (action)
		{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				PROPAGATE_GLFW_EVENT(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				PROPAGATE_GLFW_EVENT(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				PROPAGATE_GLFW_EVENT(event);
				break;
			}
		}
	}
	void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		switch (action)
		{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				PROPAGATE_GLFW_EVENT(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				PROPAGATE_GLFW_EVENT(event);
				break;
			}
		}
	}
	void GlfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		MouseScrolledEvent event(xOffset, yOffset);
		PROPAGATE_GLFW_EVENT(event);
	}
	void GlfwCursorPosCallback(GLFWwindow* window, double xPos, double yPos)
	{
		MouseMovedEvent event(xPos, yPos);
		PROPAGATE_GLFW_EVENT(event);
	}

	WindowsWindow::WindowsWindow(WindowData& data) : Window(data)
	{
		if (!s_glfwInitialized)
		{
			s_glfwInitialized = glfwInit();
			EK_ASSERT(s_glfwInitialized, "Failed to initialize GLFW!");
		}

		if (Renderer::GetAPI() == ApiType::OpenGL)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		}
		else
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title, nullptr, nullptr);
		glfwSetWindowTitle(m_window, m_data.title);

		if (Renderer::GetAPI() == ApiType::OpenGL)
		{
			glfwMakeContextCurrent(m_window);
			glfwSwapInterval(0);
		}
		
		glfwSetWindowUserPointer(m_window, &m_data);

		glfwSetErrorCallback(GlfwErrorCallback);
		glfwSetWindowSizeCallback(m_window, GlfwWindowSizeCallback);
		//glfwSetFramebufferSizeCallback(m_window, GlfwFramebufferSizeCallback);
		glfwSetWindowCloseCallback(m_window, GlfwWindowCloseCallback);
		glfwSetKeyCallback(m_window, GlfwKeyCallback);
		glfwSetMouseButtonCallback(m_window, GlfwMouseButtonCallback);
		glfwSetScrollCallback(m_window, GlfwScrollCallback);
		glfwSetCursorPosCallback(m_window, GlfwCursorPosCallback);

		EK_CORE_INFO("Initialized window");
	}

	WindowsWindow::~WindowsWindow() {}
	void WindowsWindow::Shutdown()
	{
		EK_CORE_INFO("Window shutdown");
		glfwDestroyWindow(m_window);
		glfwTerminate();
		s_glfwInitialized = false;
	}
	void WindowsWindow::Update(float deltaTime)
	{
		EK_PROFILE_NAME("Window");

		Window::Update(deltaTime);
		glfwPollEvents();
	}
	void WindowsWindow::SetWindowHint(int hint, int value)
	{
		glfwWindowHint(hint, value);
	}
	void WindowsWindow::SwapBuffers()
	{
		EK_PROFILE();

		glfwSwapBuffers(m_window);
	}
	void WindowsWindow::WaitEvents()
	{
		glfwWaitEvents();
	}
	GLFWwindow* WindowsWindow::GetGlfwWindow()
	{
		return m_window;
	}
}