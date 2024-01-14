#include "precompiled.h"
#include "Components.h"
#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Eklipse
{
	glm::mat4 TransformComponent::GetTransformMatrix() const
	{
		EK_PROFILE();

		auto& translation = glm::translate(glm::mat4(1.0f), transform.position);

		/*auto& rotation = glm::rotate(glm::mat4(1.0f), glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotation = glm::rotate(rotation, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotation = glm::rotate(rotation, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));*/
		auto& rotation = glm::toMat4(glm::quat(glm::radians(transform.rotation)));

		auto& scale = glm::scale(glm::mat4(1.0f), transform.scale);

		// TODO: Should set transformMatrix here, but it's not working for some reason
		return translation * rotation * scale;
	}
	void ScriptComponent::SetScript(const std::string& name, const EklipseEngine::Reflections::ClassInfo& info, Entity entity)
	{
		if (info.create != nullptr)
		{
			script = info.create(CreateRef<Entity>(entity));
			scriptName = name; 
			classInfo = info; 
		}
		else
		{
			EK_ERROR("Script '{0}' does not have a create function!", name);
		}
	}
	void ScriptComponent::DestroyScript()
	{
		delete script;
		script = nullptr;
	}
}