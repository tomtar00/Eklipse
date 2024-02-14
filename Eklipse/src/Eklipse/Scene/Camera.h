#pragma once
#include <glm/glm.hpp>
#include "Transform.h"

namespace Eklipse
{
	class EK_API Camera
	{
	public:
		Camera() = default;
		void UpdateViewProjectionMatrix(Transform& transform, float aspectRatio);

		const glm::mat4& GetViewProjectionMatrix() const;
		const glm::mat4& GetViewMatrix() const;
		const glm::mat4& GetProjectionMatrix() const;

		float m_fov = 120.0f;
		float m_nearPlane = 0.1f;
		float m_farPlane = 100.0f;
		bool m_isMain;
	private:
		glm::mat4 m_viewProj;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;
	};
}