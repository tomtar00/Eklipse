#include <precompiled.h>
#include "OpenGLAPI.h"
#include <glad/glad.h>

#include "GL.h"
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

		OpenGLAPI::OpenGLAPI()
		{
			s_instance = this;
		}
		OpenGLAPI& OpenGLAPI::Get()
		{
			return *s_instance;
		}
		void OpenGLAPI::Init()
		{
			if (m_initialized)
			{
				EK_CORE_WARN("OpenGL API already initialized!");
				return;
			}

			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			EK_ASSERT(status, "Failed to initialize Glad!");

#ifdef EK_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LINE_SMOOTH);

			std::stringstream ss;
			ss << glGetString(GL_VERSION);
			EK_CORE_INFO("OpenGL initialized - {0}", ss.str());
			m_initialized = true;
		}
		void OpenGLAPI::Shutdown()
		{
			if (!m_initialized)
			{
				EK_CORE_WARN("OpenGL API has already shut down!");
				return;
			}

			EK_CORE_INFO("Shutdown OpenGL");
			m_initialized = false;
		}
		void OpenGLAPI::BeginFrame()
		{
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
		void OpenGLAPI::EndFrame()
		{
			Application::Get().GetWindow()->SwapBuffers();
		}
		void OpenGLAPI::DrawIndexed(const Entity& entity)
		{
			entity.m_vertexArray->Bind();
			entity.m_uniformBuffer->SetData(&entity.m_ubo, sizeof(entity.m_ubo));

			uint32_t numIndices = entity.m_vertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
		}
		float OpenGLAPI::GetAspectRatio()
		{
			return (float)g_viewportSize.width / (float)g_viewportSize.height;
		}
	}
}