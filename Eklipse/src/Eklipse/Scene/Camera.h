#pragma once
#include <glm/glm.hpp>
#include "Transform.h"

namespace Eklipse
{
	class Camera
	{
	public:
		Camera() = default;
		void UpdateViewProjectionMatrix(Transform transform, float aspectRatio);

		inline const glm::mat4& GetViewProjectionMatrix() const { return m_viewProj; }
		inline const glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
		inline const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }

		float m_fov = 120.0f;
		float m_nearPlane = 0.1f;
		float m_farPlane = 100.0f;

		inline bool* IsMain() { return &m_isMain; }

	private:
		glm::mat4 m_viewProj;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;

		bool m_isMain;
	};
}