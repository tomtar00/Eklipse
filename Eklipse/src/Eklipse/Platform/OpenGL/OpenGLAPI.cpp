#include <precompiled.h>
#include "OpenGLAPI.h"
#include "GL.h"

#include <Eklipse/Core/Application.h>
#include <Eklipse/Renderer/RenderCommand.h>

#include <glad/glad.h>

namespace Eklipse
{
	namespace OpenGL
	{
		//GLFramebuffer* g_GLSceneFramebuffer = nullptr;
		std::vector<GLFramebuffer*> g_GLOffScreenFramebuffers{};

		static void OpenGLMessageCallback(
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
		bool OpenGLAPI::Init()
		{
			if (m_initialized)
			{
				EK_CORE_WARN("OpenGL API already initialized!");
				return false;
			}

			try
			{
				int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
				THROW(status, "Failed to initialize Glad!");

	#ifdef EK_DEBUG
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(OpenGLMessageCallback, nullptr);
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	#endif

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glEnable(GL_DEPTH_TEST);

				//std::vector<float> vertices = {
				//	 1.0f,  1.0f, 1.0f, 1.0f,  // top right
				//	 1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
				//	-1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
				//	-1.0f,  1.0f, 0.0f, 1.0f,  // top left
				//};
				//std::vector<uint32_t> indices = {
				//	0, 1, 3,
				//	1, 2, 3
				//};

				//Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices);
				//BufferLayout layout = {
				//	{ "inPos",			ShaderDataType::FLOAT2,		false },
				//	{ "inTexCoords",	ShaderDataType::FLOAT2,		false },
				//};
				//vertexBuffer->SetLayout(layout);

				//m_vertexArray = VertexArray::Create();
				//m_vertexArray->AddVertexBuffer(vertexBuffer);
				//m_vertexArray->SetIndexBuffer(IndexBuffer::Create(indices));

				EK_CORE_INFO("OpenGL Info:");
				EK_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
				EK_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
				EK_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
				m_initialized = true;
			}
			catch (const std::exception& e)
			{
				m_initialized = true;
				Shutdown();
				EK_CORE_ERROR("Failed to initialize OpenGL API: {0}", e.what());
				return false;
			}
		}
		void OpenGLAPI::Shutdown()
		{
			if (!m_initialized)
			{
				EK_CORE_WARN("OpenGL API has already shut down!");
				return;
			}

			//g_GLDefaultFramebuffer = nullptr;
			g_GLOffScreenFramebuffers.clear();

			EK_CORE_INFO("OpenGL shutdown");
			m_initialized = false;
		}
		void OpenGLAPI::WaitDeviceIdle()
		{
		}
		void OpenGLAPI::BeginFrame()
		{
			
		}
		void OpenGLAPI::EndFrame()
		{
			EK_PROFILE();

			/*uint32_t width = Application::Get().GetInfo().windowWidth;
			uint32_t height = Application::Get().GetInfo().windowHeight;
			glViewport(0, 0, width, height);
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
			auto& spriteShader = Application::Get().GetAssetLibrary()->GetShader("Assets/Shaders/Default2D.eksh");
			spriteShader->Bind();
			glDisable(GL_DEPTH_TEST);
			glActiveTexture(GL_TEXTURE0 + spriteShader->GetFragmentReflection().samplers[0].binding);
			glBindTexture(GL_TEXTURE_2D, g_GLDefaultFramebuffer->GetMainColorAttachment());
			RenderCommand::DrawIndexed(m_vertexArray);
			glBindTexture(GL_TEXTURE_2D, 0);*/

			Application::Get().GetWindow()->SwapBuffers();
		}
		void OpenGLAPI::DrawIndexed(Ref<VertexArray> vertexArray)
		{	
			EK_PROFILE();

			uint32_t numIndices = vertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
		}
	}
}