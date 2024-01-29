#include "precompiled.h"
#include "GLBuffers.h"

#include <glad/glad.h>

namespace Eklipse
{
	namespace OpenGL
	{
		////////////////////////////////////////////////////////////////
		// VERTEX BUFFER ///////////////////////////////////////////////
		////////////////////////////////////////////////////////////////

		GLVertexBuffer::GLVertexBuffer(const Vec<float>& vertices) : m_id(0)
		{
			m_count = vertices.size();
			glCreateBuffers(1, &m_id);
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		}
		GLVertexBuffer::~GLVertexBuffer()
		{
			Dispose();
		}
		void GLVertexBuffer::SetData(const void* data, uint32_t size)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		}
		void GLVertexBuffer::Bind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
		}
		void GLVertexBuffer::Unbind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		void GLVertexBuffer::Dispose() const
		{
			glDeleteBuffers(1, &m_id);
		}

		////////////////////////////////////////////////////////////////
		// INDEX BUFFER ////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////

		GLIndexBuffer::GLIndexBuffer(const Vec<uint32_t>& indices) : m_id(0)
		{
			m_count = indices.size();
			glCreateBuffers(1, &m_id);
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
			glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * m_count, indices.data(), GL_STATIC_DRAW);
		}
		GLIndexBuffer::~GLIndexBuffer()
		{
			Dispose();
		}
		void GLIndexBuffer::Bind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		}
		void GLIndexBuffer::Unbind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		void GLIndexBuffer::Dispose() const
		{
			glDeleteBuffers(1, &m_id);
		}

		size_t GLIndexBuffer::GetCount() const
		{
			return m_count;
		}

		////////////////////////////////////////////////////////////////
		// UNIFORM BUFFER //////////////////////////////////////////////
		////////////////////////////////////////////////////////////////

		GLUniformBuffer::GLUniformBuffer(uint32_t size, uint32_t binding) : m_id(0)
		{
			glCreateBuffers(1, &m_id);
			glNamedBufferData(m_id, size, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
		}
		GLUniformBuffer::~GLUniformBuffer()
		{
			Dispose();
		}
		void GLUniformBuffer::Dispose() const
		{
			glDeleteBuffers(1, &m_id);
		}
		void GLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
		{
			glNamedBufferSubData(m_id, offset, size, data);
		}
		void* GLUniformBuffer::GetBuffer() const
		{
			return nullptr;
		}
	}
}