#pragma once
#include <vector>
#include "Shader.h"

namespace Eklipse
{
	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::FLOAT:    return 4;
			case ShaderDataType::FLOAT2:   return 4 * 2;
			case ShaderDataType::FLOAT3:   return 4 * 3;
			case ShaderDataType::FLOAT4:   return 4 * 4;
			case ShaderDataType::MAT3:     return 4 * 3 * 3;
			case ShaderDataType::MAT4:     return 4 * 4 * 4;
			case ShaderDataType::INT:      return 4;
			case ShaderDataType::INT2:     return 4 * 2;
			case ShaderDataType::INT3:     return 4 * 3;
			case ShaderDataType::INT4:     return 4 * 4;
			case ShaderDataType::BOOL:     return 1;
		}

		EK_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		String name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		BufferElement() = default;
		BufferElement(const String& name, ShaderDataType type, bool normalized = false)
			: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

		uint32_t GetComponentCount() const;
	};

	class EK_API BufferLayout
	{
	public:
		BufferLayout() : m_stride(0) {}
		BufferLayout(std::initializer_list<BufferElement> elements);

		inline uint32_t GetStride() const { return m_stride; }
		inline const Vec<BufferElement>& GetElements() const { return m_elements; }

		Vec<BufferElement>::iterator begin() { return m_elements.begin(); }
		Vec<BufferElement>::iterator end() { return m_elements.end(); }
		Vec<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		Vec<BufferElement>::const_iterator end() const { return m_elements.end(); }

	private:
		void CalculateOffsetAndStride();

	private:
		Vec<BufferElement> m_elements;
		uint32_t m_stride;
	};

	class EK_API VertexBuffer
	{
	public:
		static Ref<VertexBuffer> Create(const Vec<float>& vertices);
		virtual ~VertexBuffer() = default;

		virtual void SetData(const void* data, uint32_t size) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

		inline const uint32_t GetCount() const { return m_count; }
		inline const BufferLayout& GetBufferLayout() const { return m_bufferLayout; }
		inline void SetLayout(const BufferLayout& layout) { m_bufferLayout = layout; }

	protected:
		BufferLayout m_bufferLayout;
		uint32_t m_count;
	};

	class EK_API IndexBuffer
	{
	public:
		static Ref<IndexBuffer> Create(const Vec<uint32_t>& indices);
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

		virtual size_t GetCount() const = 0;

	protected:
		size_t m_count;
	};

	class EK_API UniformBuffer
	{
	public:
		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
		virtual ~UniformBuffer() = default;

		virtual void Dispose() const = 0;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		
		virtual void* GetBuffer() const = 0;
	};
}