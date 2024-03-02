#pragma once
#include <glad/glad.h>
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

			bool Init() override;
			void Shutdown() override;
			void WaitDeviceIdle() override;

			void BeginFrame() override;
			void Submit() override;

			virtual void SetPipelineTopologyMode(Pipeline::TopologyMode topologyMode) override;
			virtual void SetPipelineType(Pipeline::Type type) override;

			virtual void DrawIndexed(Ref<VertexArray> vertexArray) override;

		private:
			inline static OpenGLAPI* s_instance = nullptr;

			Ref<VertexArray> m_fullscreenVA;
			Ref<Shader> m_fullscreenShader;

			GLint m_topologyMode;
		};
	}
}