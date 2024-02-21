#include "precompiled.h"
#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Eklipse
{
	glm::vec3 Transform::GetForward() const
	{
		auto& _rotation = glm::toMat4(glm::quat(glm::radians(rotation)));
		return -glm::vec3(_rotation[2]);
	}
	glm::vec3 Transform::GetRight() const
	{
		auto& _rotation = glm::toMat4(glm::quat(glm::radians(rotation)));
		return glm::vec3(_rotation[0]);
	}
	glm::vec3 Transform::GetUp() const
	{
		auto& _rotation = glm::toMat4(glm::quat(glm::radians(rotation)));
		return glm::vec3(_rotation[1]);
	}
}