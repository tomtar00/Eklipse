#include "precompiled.h"
#include "GLVertexArray.h"

namespace Eklipse
{
	namespace OpenGL
	{
		static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::FLOAT:    return GL_FLOAT;
				case ShaderDataType::FLOAT2:   return GL_FLOAT;
				case ShaderDataType::FLOAT3:   return GL_FLOAT;
				case ShaderDataType::FLOAT4:   return GL_FLOAT;
				case ShaderDataType::MAT3:     return GL_FLOAT;
				case ShaderDataType::MAT4:     return GL_FLOAT;
				case ShaderDataType::INT:      return GL_INT;
				case ShaderDataType::INT2:     return GL_INT;
				case ShaderDataType::INT3:     return GL_INT;
				case ShaderDataType::INT4:     return GL_INT;
				case ShaderDataType::BOOL:     return GL_BOOL;
			}

			EK_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}

		GLVertexArray::GLVertexArray() : m_id(0), m_vertexBufferIdx(0)
		{
			glGenVertexArrays(1, &m_id);
		}
		void GLVertexArray::Bind() const
		{
			glBindVertexArray(m_id);
		}
		void GLVertexArray::Unbind() const
		{
			glBindVertexArray(0);
		}
		void GLVertexArray::Dispose() const
		{
			glDeleteVertexArrays(1, &m_id);
		}
		void GLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
		{
			EK_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!");

			glBindVertexArray(m_id);
			vertexBuffer->Bind();

			uint32_t vertexElementsCounter = 0;
			const auto& layout = vertexBuffer->GetBufferLayout();
			for (const auto& element : layout)
			{
				vertexElementsCounter += element.GetComponentCount();
				switch (element.type)
				{
					case ShaderDataType::FLOAT:
					case ShaderDataType::FLOAT2:
					case ShaderDataType::FLOAT3:
					case ShaderDataType::FLOAT4:
					{
						glEnableVertexAttribArray(m_vertexBufferIdx);
						glVertexAttribPointer(
							m_vertexBufferIdx,
							element.GetComponentCount(),
							ShaderDataTypeToOpenGLBaseType(element.type),
							element.normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)element.offset
						);
						m_vertexBufferIdx++;
						break;
					}
					case ShaderDataType::INT:
					case ShaderDataType::INT2:
					case ShaderDataType::INT3:
					case ShaderDataType::INT4:
					case ShaderDataType::BOOL:
					{
						glEnableVertexAttribArray(m_vertexBufferIdx);
						glVertexAttribIPointer(
							m_vertexBufferIdx,
							element.GetComponentCount(),
							ShaderDataTypeToOpenGLBaseType(element.type),
							layout.GetStride(),
							(const void*)element.offset
						);
						m_vertexBufferIdx++;
						break;
					}
					case ShaderDataType::MAT3:
					case ShaderDataType::MAT4:
					{
						uint8_t count = element.GetComponentCount();
						for (uint8_t i = 0; i < count; i++)
						{
							glEnableVertexAttribArray(m_vertexBufferIdx);
							glVertexAttribPointer(
								m_vertexBufferIdx,
								count,
								ShaderDataTypeToOpenGLBaseType(element.type),
								element.normalized ? GL_TRUE : GL_FALSE,
								layout.GetStride(),
								(const void*)(element.offset + sizeof(float) * count * i)
							);
							glVertexAttribDivisor(m_vertexBufferIdx, 1);
							m_vertexBufferIdx++;
						}
						break;
					}
					default:
						EK_ASSERT(false, "Unknown ShaderDataType!");
				}
			}

			m_totalNumVertices += vertexBuffer->GetCount() / vertexElementsCounter;
			m_vertexBuffers.push_back(vertexBuffer);
		}
		void GLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
		{
			glBindVertexArray(m_id);
			indexBuffer->Bind();

			m_indexBuffer = indexBuffer;
		}
	}
}
