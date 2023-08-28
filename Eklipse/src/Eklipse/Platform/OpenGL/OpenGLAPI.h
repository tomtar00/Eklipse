#pragma once
#include <Eklipse/Renderer/GraphicsAPI.h>
#include <Eklipse/Platform/Windows/WindowsWindow.h>
#include "GLEntity.h"

namespace Eklipse
{
	namespace OpenGL
	{
		class OpenGLAPI : public GraphicsAPI
		{
		public:
			OpenGLAPI();
			static OpenGLAPI& Get();

			void Init(Scene* scene) override;
			void Shutdown() override;

			void BeginFrame() override;
			void DrawFrame() override;
			void EndFrame() override;

			float GetAspectRatio() override;

		private:
			inline static OpenGLAPI* s_instance = nullptr;
			GLFWwindow* m_glfwWindow;
			GLEntityManager m_entityManager;
		};
	}
}