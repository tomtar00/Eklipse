#include "precompiled.h"
#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Eklipse
{
	int Entity::s_idCounter = 0;

	Entity::Entity(std::string name) : m_name(name)
	{
		m_id = s_idCounter++;
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
