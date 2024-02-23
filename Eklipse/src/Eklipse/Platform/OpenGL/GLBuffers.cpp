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
			EK_CORE_PROFILE();
			m_count = vertices.size();
			glCreateBuffers(1, &m_id);
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
		}
		GLVertexBuffer::~GLVertexBuffer()
		{
			EK_CORE_PROFILE();
			Dispose();
		}
		void GLVertexBuffer::SetData(const void* data, uint32_t size)
		{
			EK_CORE_PROFILE();
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		}
		void GLVertexBuffer::Bind() const
		{
			EK_CORE_PROFILE();
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
		}
		void GLVertexBuffer::Unbind() const
		{
			EK_CORE_PROFILE();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		void GLVertexBuffer::Dispose() const
		{
			EK_CORE_PROFILE();
			glDeleteBuffers(1, &m_id);
		}

		////////////////////////////////////////////////////////////////
		// INDEX BUFFER ////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////

		GLIndexBuffer::GLIndexBuffer(const Vec<uint32_t>& indices) : m_id(0)
		{
			EK_CORE_PROFILE();
			m_count = indices.size();
			glCreateBuffers(1, &m_id);
			glBindBuffer(GL_ARRAY_BUFFER, m_id);
			glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * m_count, indices.data(), GL_STATIC_DRAW);
		}
		GLIndexBuffer::~GLIndexBuffer()
		{
			EK_CORE_PROFILE();
			Dispose();
		}
		void GLIndexBuffer::Bind() const
		{
			EK_CORE_PROFILE();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		}
		void GLIndexBuffer::Unbind() const
		{
			EK_CORE_PROFILE();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		void GLIndexBuffer::Dispose() const
		{
			EK_CORE_PROFILE();
			glDeleteBuffers(1, &m_id);
		}

		size_t GLIndexBuffer::GetCount() const
		{
			return m_count;
		}

		////////////////////////////////////////////////////////////////
		// UNIFORM BUFFER //////////////////////////////////////////////
		////////////////////////////////////////////////////////////////

		GLUniformBuffer::GLUniformBuffer(size_t size, uint32_t binding) : m_id(0)
		{
			EK_CORE_PROFILE();
			glCreateBuffers(1, &m_id);
			glNamedBufferData(m_id, size, nullptr, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
		}
		GLUniformBuffer::~GLUniformBuffer()
		{
			EK_CORE_PROFILE();
			Dispose();
		}
		void GLUniformBuffer::Dispose() const
		{
			EK_CORE_PROFILE();
			glDeleteBuffers(1, &m_id);
		}
		void GLUniformBuffer::SetData(const void* data, size_t size, uint32_t offset)
		{
			EK_CORE_PROFILE();
			glNamedBufferSubData(m_id, offset, size, data);
		}
		void* GLUniformBuffer::GetBuffer() const
		{
			return nullptr;
		}

		////////////////////////////////////////////////////////////////
		// STORAGE BUFFER //////////////////////////////////////////////
		////////////////////////////////////////////////////////////////

		GLStorageBuffer::GLStorageBuffer(size_t size, uint32_t binding)
		{
		    EK_CORE_PROFILE();
			m_size = size;
            glCreateBuffers(1, &m_id);
            glNamedBufferData(m_id, size, nullptr, GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_id);
		}
		GLStorageBuffer::~GLStorageBuffer()
		{
		    EK_CORE_PROFILE();
            Dispose();
		}
		void GLStorageBuffer::Dispose() const
		{
            EK_CORE_PROFILE();
            glDeleteBuffers(1, &m_id);
		}
		void GLStorageBuffer::SetData(const void* data, size_t size, uint32_t offset)
		{
			EK_CORE_PROFILE();
            glNamedBufferSubData(m_id, offset, size, data);
		}
		void* GLStorageBuffer::GetBuffer() const
		{
			return (void*)&m_id;
		}
	}
}