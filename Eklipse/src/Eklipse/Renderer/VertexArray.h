#pragma once
#include "Buffers.h"

namespace Eklipse
{
	class EK_API VertexArray
	{
	public:
		static Ref<VertexArray> Create();
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		inline const Vec<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_vertexBuffers; }
		inline const Ref<IndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }
		inline const uint32_t GetTotalNumVertices() const { return m_totalNumVertices; }

	protected:
		Vec<Ref<VertexBuffer>> m_vertexBuffers;
		Ref<IndexBuffer> m_indexBuffer;
		uint32_t m_totalNumVertices{0};
	};
}