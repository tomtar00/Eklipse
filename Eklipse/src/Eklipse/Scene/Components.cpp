#include "precompiled.h"
#include "Components.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Eklipse
{
	glm::mat4& TransformComponent::GetTransformMatrix() const
	{
		EK_PROFILE();

		glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
		model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, transform.scale);

		return model;
	}
}
