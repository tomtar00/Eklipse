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
	static void GlfwErrorCallback(int error, const char* description)
	{
		EK_CORE_PROFILE();
		EK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}
	static void GlfwWindowSizeCallback(GLFWwindow* window, int width, int height)
	{
		EK_CORE_PROFILE();
		WindowResizeEvent event(width, height);
		PROPAGATE_GLFW_EVENT(event);
	}
	static void GlfwWindowCloseCallback(GLFWwindow* window)
	{
		EK_CORE_PROFILE();
		WindowCloseEvent event;
		PROPAGATE_GLFW_EVENT(event);
	}
	static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		EK_CORE_PROFILE();
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
	static void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		EK_CORE_PROFILE();
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
	static void GlfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		EK_CORE_PROFILE();
		MouseScrolledEvent event(xOffset, yOffset);
		PROPAGATE_GLFW_EVENT(event);
	}
	static void GlfwCursorPosCallback(GLFWwindow* window, double xPos, double yPos)
	{
		EK_CORE_PROFILE();
		MouseMovedEvent event(xPos, yPos);
		PROPAGATE_GLFW_EVENT(event);
	}
	static void GlfwWindowFocusCallback(GLFWwindow* window, int focused)
	{
		EK_CORE_PROFILE();
		if (focused)
		{
			WindowFocusEvent event;
			PROPAGATE_GLFW_EVENT(event);
		}
		else
		{
			WindowLostFocusEvent event;
			PROPAGATE_GLFW_EVENT(event);
		}
	}

	WindowsWindow::WindowsWindow(WindowData& data) : Window(data)
	{
		EK_CORE_PROFILE();
		if (!s_glfwInitialized)
		{
			s_glfwInitialized = glfwInit();
			EK_ASSERT(s_glfwInitialized, "Failed to initialize GLFW!");
		}

		if (Renderer::GetAPI() == GraphicsAPI::Type::OpenGL)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		}
		else
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title, nullptr, nullptr);
		glfwSetWindowTitle(m_window, m_data.title);

		if (Renderer::GetAPI() == GraphicsAPI::Type::OpenGL)
		{
			glfwMakeContextCurrent(m_window);
			glfwSwapInterval(0);
		}

		glfwSetWindowUserPointer(m_window, &m_data);

		glfwSetErrorCallback(GlfwErrorCallback);
		glfwSetWindowSizeCallback(m_window, GlfwWindowSizeCallback);
		glfwSetWindowCloseCallback(m_window, GlfwWindowCloseCallback);
		glfwSetKeyCallback(m_window, GlfwKeyCallback);
		glfwSetMouseButtonCallback(m_window, GlfwMouseButtonCallback);
		glfwSetScrollCallback(m_window, GlfwScrollCallback);
		glfwSetCursorPosCallback(m_window, GlfwCursorPosCallback);
		glfwSetWindowFocusCallback(m_window, GlfwWindowFocusCallback);

		EK_CORE_DBG("Window initialized");
	}

	WindowsWindow::~WindowsWindow() 
	{
		EK_CORE_PROFILE();
		Shutdown();
	}
	void WindowsWindow::Shutdown()
	{
		EK_CORE_PROFILE();
		EK_CORE_DBG("Window shutdown");
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
		EK_CORE_PROFILE();
		glfwWindowHint(hint, value);
	}
	void WindowsWindow::SwapBuffers()
	{
		EK_CORE_PROFILE();
		glfwSwapBuffers(m_window);
	}
	void WindowsWindow::WaitEvents()
	{
		EK_CORE_PROFILE();
		glfwWaitEvents();
	}
	void WindowsWindow::SetTitle(const char* title)
	{
		EK_CORE_PROFILE();
		EK_CORE_TRACE("Setting window title to '{0}'", title);
		glfwSetWindowTitle(m_window, title);
		EK_CORE_DBG("Window title set to '{0}'", title);
	}
	void WindowsWindow::Maximize()
	{
		EK_CORE_PROFILE();
		EK_CORE_TRACE("Maximizing window");
        glfwMaximizeWindow(m_window);
		EK_CORE_DBG("Window maximized");
	}
	bool WindowsWindow::IsMaximized()
	{
	    return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
	}
	void WindowsWindow::SetCursorMode(CursorMode mode)
	{
		EK_CORE_PROFILE();
        switch (mode)
        {
            case CursorMode::Normal:
            {
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            }
            case CursorMode::Hidden:
            {
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
            }
            case CursorMode::Disabled:
            {
                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
            }
        }
	}
	GLFWwindow* WindowsWindow::GetGlfwWindow()
	{
		return m_window;
	}
}