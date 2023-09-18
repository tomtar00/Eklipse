#pragma once
#include <vector>
#include "Vertex.h"

namespace Eklipse
{
	enum class ShaderDataType
	{
		None = 0,
		Bool,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat3, Mat4
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return 4;
			case ShaderDataType::Float2:   return 4 * 2;
			case ShaderDataType::Float3:   return 4 * 3;
			case ShaderDataType::Float4:   return 4 * 4;
			case ShaderDataType::Mat3:     return 4 * 3 * 3;
			case ShaderDataType::Mat4:     return 4 * 4 * 4;
			case ShaderDataType::Int:      return 4;
			case ShaderDataType::Int2:     return 4 * 2;
			case ShaderDataType::Int3:     return 4 * 3;
			case ShaderDataType::Int4:     return 4 * 4;
			case ShaderDataType::Bool:     return 1;
		}

		EK_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		BufferElement() = default;
		BufferElement(const std::string& name, ShaderDataType type, bool normalized = false)
			: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

		uint32_t GetComponentCount() const;
	};

	class BufferLayout
	{
	public:
		BufferLayout() : m_stride(0) {}
		BufferLayout(std::initializer_list<BufferElement> elements);

		inline uint32_t GetStride() const { return m_stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_elements; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

	private:
		void CalculateOffsetAndStride();

	private:
		std::vector<BufferElement> m_elements;
		uint32_t m_stride;
	};

	class VertexBuffer
	{
	public:
		static Ref<VertexBuffer> Create(const std::vector<float>& vertices);
		virtual ~VertexBuffer() = default;

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

	class IndexBuffer
	{
	public:
		static Ref<IndexBuffer> Create(const std::vector<uint32_t>& indices);
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Dispose() const = 0;

		virtual size_t GetCount() const = 0;

	protected:
		size_t m_count;
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