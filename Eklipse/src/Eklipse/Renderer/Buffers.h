#pragma once
#include <vector>
#include "Vertex.h"

namespace Eklipse
{
	class VertexBuffer
	{
	public:
		static Ref<VertexBuffer> Create(std::vector<Vertex> vertices);
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;
	};

	class IndexBuffer
	{
	public:
		static Ref<IndexBuffer> Create(std::vector<uint32_t> indices);
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

		virtual uint32_t GetCount() const = 0;
	protected:
		uint32_t m_count;
	};

	class UniformBuffer
	{
	public:
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
		virtual ~UniformBuffer() = default;

		virtual void Dispose() const = 0;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		
		virtual void* GetBuffer() const = 0;
	};
}