#pragma once
#include "Transform.h"
#include "Entity.h"

namespace Eklipse
{
	class Camera : public Entity
	{
	public:
		Camera() = default;
		void UpdateViewProjectionMatrix(float aspectRatio);

		glm::mat4 m_viewProj;
		float m_fov = 120.0f;
		float m_nearPlane = 0.1f;
		float m_farPlane = 100.0f;
	};
}