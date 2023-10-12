#pragma once
#include <Eklipse/Renderer/GraphicsAPI.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class OpenGLAPI : public GraphicsAPI
		{
		public:
			OpenGLAPI();
			static OpenGLAPI& Get();

			void Init() override;
			void Shutdown() override;

			void BeginFrame() override;
			void EndFrame() override;

			//virtual void BeginGeometryPass() override {};
			//virtual void BeginGUIPass() override {};
			//virtual void EndPass() override {};

			virtual void DrawIndexed(Ref<VertexArray> vertexArray) override;
			virtual void SetSceneFramebuffer(Ref<Framebuffer> framebuffer) override;
			virtual void SetGUIFramebuffer(Ref<Framebuffer> framebuffer) override;

		private:
			inline static OpenGLAPI* s_instance = nullptr;
			Ref<VertexArray> m_vertexArray;
		};
	}
}