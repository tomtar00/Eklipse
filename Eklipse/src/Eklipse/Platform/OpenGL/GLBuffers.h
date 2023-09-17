#pragma once
#include <Eklipse/Renderer/Vertex.h>
#include <Eklipse/Renderer/Buffers.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLVertexBuffer : public Eklipse::VertexBuffer
		{
		public:
			GLVertexBuffer(const std::vector<float>& vertices);

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

		private:
			uint32_t m_id;
		};

		class GLIndexBuffer : public Eklipse::IndexBuffer
		{
		public:
			GLIndexBuffer(const std::vector<uint32_t>& indices);

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;
			virtual size_t GetCount() const override;
		private:
			uint32_t m_id;
		};

		class GLUniformBuffer : public Eklipse::UniformBuffer
		{
		public:
			GLUniformBuffer(uint32_t size, uint32_t binding);

			virtual void Dispose() const override;
			virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
			virtual void* GetBuffer() const override;
		private:
			uint32_t m_id;
		};
	}
}