#pragma once
#include "Transform.h"

namespace Eklipse
{
	class Camera
	{
	public:
		void UpdateViewProjectionMatrix(float aspectRatio);

		Transform m_transform;
		glm::mat4 m_viewProj;
		float m_fov = 120.0f;
		float m_nearPlane = 0.1f;
		float m_farPlane = 100.0f;
	};
}