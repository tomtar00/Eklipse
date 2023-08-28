#include "precompiled.h"
#include "WindowsWindow.h"

#include <Eklipse/Events/ApplicationEvent.h>
#include <Eklipse/Events/KeyEvent.h>
#include <Eklipse/Events/MouseEvent.h>

#include <Eklipse/Utils/Stats.h>

#define GLFW_INCLUDE_VULKAN

namespace Eklipse
{
#pragma region GLFW callbacks

#define PROPAGATE_EVENT(x) ((WindowData*)glfwGetWindowUserPointer(window))->EventCallback(x)

	void GlfwErrorCallback(int error, const char* description)
	{
		EK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	void GlfwWindowSizeCallback(GLFWwindow* window, int width, int height)
	{
		bool minimized = width == 0 && height == 0;
		bool* isMinimized = &((WindowData*)glfwGetWindowUserPointer(window))->minimized;

		if (minimized)
		{
			WindowMinimizedEvent event;
			PROPAGATE_EVENT(event);
		}
		else if (*isMinimized && !minimized)
		{
			WindowMaximizedEvent event;
			PROPAGATE_EVENT(event);
		}
		else
		{
			WindowResizeEvent event(width, height);
			PROPAGATE_EVENT(event);
		}
		*isMinimized = minimized;
	}
	void GlfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		FramebufferResizeEvent event(width, height);

		((WindowData*)glfwGetWindowUserPointer(window))->framebufferResized = true;
		PROPAGATE_EVENT(event);
	}
	void GlfwWindowCloseCallback(GLFWwindow* window)
	{
		WindowCloseEvent event;
		PROPAGATE_EVENT(event);
	}

	void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		switch (action)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(key, 0);
			PROPAGATE_EVENT(event);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(key);
			PROPAGATE_EVENT(event);
			break;
		}
		case GLFW_REPEAT:
		{
			KeyPressedEvent event(key, 1);
			PROPAGATE_EVENT(event);
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
			PROPAGATE_EVENT(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(button);
			PROPAGATE_EVENT(event);
			break;
		}
		}
	}
	void GlfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		MouseScrolledEvent event(xOffset, yOffset);
		PROPAGATE_EVENT(event);
	}
	void GlfwCursorPosCallback(GLFWwindow* window, double xPos, double yPos)
	{
		MouseMovedEvent event(xPos, yPos);
		PROPAGATE_EVENT(event);
	}
#pragma endregion

	Window* Window::Create(WindowData& data)
	{
		return new WindowsWindow(data);
	}

	WindowsWindow::WindowsWindow(WindowData& data)
	{
		m_data = data;
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init()
	{
		if (!s_glfwInitialized)
		{
			int initialized = glfwInit();
			EK_ASSERT(initialized, "Failed to intialize GLFW!");

			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

			s_glfwInitialized = true;
		}
		m_window = glfwCreateWindow(m_data.width, m_data.height, m_data.title, nullptr, nullptr);
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, &m_data);

		glfwSetErrorCallback(GlfwErrorCallback);
		glfwSetWindowSizeCallback(m_window, GlfwWindowSizeCallback);
		glfwSetFramebufferSizeCallback(m_window, GlfwFramebufferSizeCallback);
		glfwSetWindowCloseCallback(m_window, GlfwWindowCloseCallback);
		glfwSetKeyCallback(m_window, GlfwKeyCallback);
		glfwSetMouseButtonCallback(m_window, GlfwMouseButtonCallback);
		glfwSetScrollCallback(m_window, GlfwScrollCallback);
		glfwSetCursorPosCallback(m_window, GlfwCursorPosCallback);

		EK_CORE_DEBUG("Initialized window");
	};

	void WindowsWindow::Shutdown()
	{
		EK_CORE_DEBUG("Window shutdown");
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void WindowsWindow::Update(float deltaTime)
	{
		std::stringstream title;
		title << m_data.title << " FPS: " << Stats::Get().fps;
		glfwSetWindowTitle(m_window, title.str().c_str());
		glfwPollEvents();
	}

	GLFWwindow* WindowsWindow::GetGlfwWindow()
	{
		return m_window;
	}
}