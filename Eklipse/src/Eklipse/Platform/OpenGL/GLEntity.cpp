#include <precompiled.h>
#include "GLEntity.h"

#include <glad/glad.h>

namespace Eklipse
{
	namespace OpenGL
	{
		GLEntityWrapper::GLEntityWrapper(Entity& entity) : m_entity(entity)
		{
			m_vertexBuffer = VertexBuffer::Create(entity.m_mesh.m_vertices);
			m_indexBuffer = IndexBuffer::Create(entity.m_mesh.m_indices);
			m_uniformBuffer = UniformBuffer::Create(sizeof(entity.m_ubo), 0);
			// m_texture.Load(entity.m_mesh.m_textureData);
		}
		void GLEntityWrapper::Dispose()
		{
			m_vertexBuffer->Dispose();
			m_indexBuffer->Dispose();
		}
		void GLEntityWrapper::Bind()
		{
			m_vertexBuffer->Bind();
			m_indexBuffer->Bind();

			m_uniformBuffer->SetData(&m_entity.m_ubo, sizeof(m_entity.m_ubo));
		}
		void GLEntityWrapper::Draw()
		{
			uint32_t numIndices = m_indexBuffer->GetCount();
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
		}

		// MANAGER //////////////////////////////////////

		void GLEntityManager::Setup(Scene* scene)
		{
			for (auto& model : scene->m_entities)
			{
				GLEntityWrapper wrapper{ model };
				m_wrappers.push_back(wrapper);
			}
		}
		void GLEntityManager::Dispose()
		{
			for (auto& wrapper : m_wrappers)
			{
				wrapper.Dispose();
			}
		}
	}
}