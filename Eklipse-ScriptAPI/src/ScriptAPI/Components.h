#pragma once
#include <glm/glm.hpp>
#include "Core.h"

namespace EklipseEngine
{
	struct EK_API Transform
	{
	private:
		class TransformImpl;
	public:
		Transform(Ref<TransformImpl> impl);

		glm::vec3& GetPosition();
		glm::vec3& GetRotation();
		glm::vec3& GetScale();

		void SetPosition(glm::vec3& position);
		void SetRotation(glm::vec3& rotation);
		void SetScale(glm::vec3& scale);

		void Translate(glm::vec3& translation);
		void Rotate(glm::vec3& rotation);
		void Scale(glm::vec3& scale);

	private:
		Ref<TransformImpl> _impl;
	};
}