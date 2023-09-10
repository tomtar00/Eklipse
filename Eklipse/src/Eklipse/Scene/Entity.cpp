#include "precompiled.h"
#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Eklipse
{
	Entity::Entity(const std::string& name, Mesh& mesh) : m_name(name), m_mesh(mesh)
	{
		m_id = s_idCounter++;

		m_vertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(m_mesh.m_vertices);

		BufferLayout layout = {
			{ "inPosition", ShaderDataType::Float3, false },
			{ "inColor",	ShaderDataType::Float3, false },
			{ "inTexCoord", ShaderDataType::Float2, false }
		};

		vertexBuffer->SetLayout(layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);
		m_vertexArray->SetIndexBuffer(IndexBuffer::Create(m_mesh.m_indices));

		m_uniformBuffer = UniformBuffer::Create(sizeof(m_ubo), 0);
		//m_texture = Texture3D::Create();
	}
	void Entity::UpdateModelMatrix(glm::mat4 viewProjMatrix)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_transform.position);
		model = glm::rotate(model, glm::radians(m_transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(m_transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(m_transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, m_transform.scale);

		glm::mat4 mvp = viewProjMatrix * model;
		m_ubo.mvp = mvp;
	}
}
