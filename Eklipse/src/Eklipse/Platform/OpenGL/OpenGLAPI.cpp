#include <precompiled.h>
#include "OpenGLAPI.h"
#include <glad/glad.h>
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	namespace OpenGL
	{
		void OpenGLMessageCallback(
			unsigned source,
			unsigned type,
			unsigned id,
			unsigned severity,
			int length,
			const char* message,
			const void* userParam)
		{
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:         EK_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       EK_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_LOW:          EK_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: EK_CORE_TRACE(message); return;
			}

			EK_ASSERT(false, "Unknown severity level!");
		}

		OpenGLAPI::OpenGLAPI() : m_glfwWindow(nullptr)
		{
			s_instance = this;
		}
		OpenGLAPI& OpenGLAPI::Get()
		{
			return *s_instance;
		}
		void OpenGLAPI::Init(Scene* scene)
		{
			if (m_initialized)
			{
				EK_CORE_WARN("OpenGL API already initialized!");
				return;
			}

			m_glfwWindow = dynamic_cast<WindowsWindow*>(Application::Get().GetWindow())->GetGlfwWindow();
			EK_ASSERT(m_glfwWindow, "Failed to get GLFW window while GL API initialization!");

			glfwMakeContextCurrent(m_glfwWindow);
			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			EK_ASSERT(status, "Failed to initialize Glad!");

#ifdef EK_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

			std::stringstream ss;
			ss << glGetString(GL_VERSION);
			EK_CORE_INFO("OpenGL initialized - {0}", ss.str());
			m_initialized = true;

			m_entityManager.Setup(scene);
		}
		void OpenGLAPI::Shutdown()
		{
			if (!m_initialized)
			{
				EK_CORE_WARN("OpenGL API has already shut down!");
				return;
			}

			m_entityManager.Dispose();

			EK_CORE_INFO("Shutdown OpenGL");
			m_initialized = false;
		}
		void OpenGLAPI::BeginFrame()
		{
			glViewport(0, 0, 1280, 720);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		void OpenGLAPI::DrawFrame()
		{
			Renderer::GetShaderLibrary().Get("geometry")->Bind();
			for (auto& wrapper : m_entityManager.m_wrappers)
			{
				wrapper.Bind();
				wrapper.Draw();
			}

			Application::Get().m_guiLayer->Draw(nullptr);
		}
		void OpenGLAPI::EndFrame()
		{
			glfwSwapBuffers(m_glfwWindow);
		}
		float OpenGLAPI::GetAspectRatio()
		{
			return 1.0f;
		}
	}
}