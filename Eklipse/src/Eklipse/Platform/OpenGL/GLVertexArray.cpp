#include "precompiled.h"
#include "GLVertexArray.h"

namespace Eklipse
{
	namespace OpenGL
	{
		GLVertexArray::GLVertexArray()
		{
			glGenVertexArrays(1, &m_id);
		}
		GLVertexArray::~GLVertexArray()
		{
			glDeleteVertexArrays(1, &m_id);
		}
		void GLVertexArray::Bind() const
		{
			glBindVertexArray(m_id);
		}
		void GLVertexArray::Unbind() const
		{
			glBindVertexArray(0);
		}
		void GLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
		{
			glBindVertexArray(m_id);
			vertexBuffer->Bind();

			// enable vertex attrib array
			// vertex attrib pointer
		}
		void GLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
		{
			glBindVertexArray(m_id);
			indexBuffer->Bind();

			m_indexBuffer = indexBuffer;
		}
	}
}
