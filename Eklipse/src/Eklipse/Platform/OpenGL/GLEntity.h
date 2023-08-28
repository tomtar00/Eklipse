#pragma once
#include <Eklipse/Scene/Scene.h>
#include <Eklipse/Renderer/Buffers.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLEntityWrapper
		{
		public:
			GLEntityWrapper(Entity& entity);
			void Dispose();

			void Bind();
			void Draw();

			Ref<VertexBuffer> m_vertexBuffer;
			Ref<IndexBuffer> m_indexBuffer;
			Ref<UniformBuffer> m_uniformBuffer;
			//Texture m_texture;

		private:
			Entity& m_entity;
		};

		class GLEntityManager
		{
		public:
			void Setup(Scene* scene);
			void Dispose();

			std::vector<GLEntityWrapper> m_wrappers;
		};
	}
}