#pragma once
#include <Eklipse/Renderer/VertexArray.h>

#include <glad/glad.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLVertexArray : public Eklipse::VertexArray
		{
		public:
			GLVertexArray();
			virtual ~GLVertexArray();

			virtual void Bind() const override;
			virtual void Unbind() const override;

			virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
			virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		private:
			uint32_t m_id;
		};
	}
}