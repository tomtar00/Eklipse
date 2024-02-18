#pragma once
#include <Eklipse/Renderer/Vertex.h>
#include <Eklipse/Renderer/Buffers.h>

namespace Eklipse
{
	namespace OpenGL
	{
		class GLVertexBuffer : public VertexBuffer
		{
		public:
			GLVertexBuffer(const Vec<float>& vertices);
			~GLVertexBuffer();

			virtual void SetData(const void* data, uint32_t size) override;
			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;

		private:
			uint32_t m_id;
		};

		class GLIndexBuffer : public IndexBuffer
		{
		public:
			GLIndexBuffer(const Vec<uint32_t>& indices);
			~GLIndexBuffer();

			virtual void Bind() const override;
			virtual void Unbind() const override;
			virtual void Dispose() const override;
			virtual size_t GetCount() const override;
		private:
			uint32_t m_id;
		};

		class GLUniformBuffer : public UniformBuffer
		{
		public:
			GLUniformBuffer(size_t size, uint32_t binding);
			~GLUniformBuffer();

			virtual void Dispose() const override;
			virtual void SetData(const void* data, size_t size, uint32_t offset = 0) override;
			virtual void* GetBuffer() const override;
		private:
			uint32_t m_id;
		};

		class GLStorageBuffer : public StorageBuffer
		{
		public:
			GLStorageBuffer(size_t size, uint32_t binding);
			~GLStorageBuffer();

			virtual void Dispose() const override;
			virtual void SetData(const void* data, size_t size, uint32_t offset = 0) override;
			virtual void* GetBuffer() const override;
		private:
			uint32_t m_id;
		};
	}
}