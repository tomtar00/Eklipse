#include <precompiled.h>
#include "OpenGLAPI.h"
#include <glad/glad.h>

#include "GL.h"
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
	namespace OpenGL
	{
		GLFramebuffer* g_sceneFramebuffer = nullptr;
		GLFramebuffer* g_guiFramebuffer = nullptr;

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

			std::vector<float> vertices = {
				 1.0f,  1.0f, 1.0f, 1.0f,  // top right
				 1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
				-1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
				-1.0f,  1.0f, 0.0f, 1.0f,  // top left
			};
			std::vector<uint32_t> indices = {
				0, 1, 3,
				1, 2, 3
			};

			Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices);
			BufferLayout layout = {
				{ "inPos",			ShaderDataType::Float2,		false },
				{ "inTexCoords",	ShaderDataType::Float2,		false },
			};
			vertexBuffer->SetLayout(layout);

			m_vertexArray = VertexArray::Create();
			m_vertexArray->AddVertexBuffer(vertexBuffer);
			m_vertexArray->SetIndexBuffer(IndexBuffer::Create(indices));

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

			EK_CORE_INFO("OpenGL shutdown");
			m_initialized = false;
		}
		void OpenGLAPI::BeginFrame()
		{
			EK_PROFILE();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		void OpenGLAPI::EndFrame()
		{
			EK_PROFILE();

			int width = Application::Get().GetInfo().windowWidth;
			int height = Application::Get().GetInfo().windowHeight;
			glViewport(0, 0, width, width);
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			ShaderLibrary::Get("sprite")->Bind(); // TODO: may take a long time to search for shader
			m_vertexArray->Bind();
			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, g_guiFramebuffer->GetMainColorAttachment());
			DrawIndexed(m_vertexArray);

			Application::Get().GetWindow()->SwapBuffers();
		}
		void OpenGLAPI::DrawIndexed(Ref<VertexArray> vertexArray)
		{	
			EK_PROFILE();

			uint32_t numIndices = vertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
		}
		void OpenGLAPI::SetSceneFramebuffer(Ref<Framebuffer> framebuffer)
		{
			g_sceneFramebuffer = static_cast<GLFramebuffer*>(framebuffer.get());
		}
		void OpenGLAPI::SetGUIFramebuffer(Ref<Framebuffer> framebuffer)
		{
			g_guiFramebuffer = static_cast<GLFramebuffer*>(framebuffer.get());
		}
	}
}