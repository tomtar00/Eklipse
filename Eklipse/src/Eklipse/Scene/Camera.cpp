#include "precompiled.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Eklipse
{
    void Camera::UpdateViewProjectionMatrix(Transform transform, float aspectRatio)
    {
        EK_PROFILE();

        // Projection
        m_projectionMatrix = glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
        //m_projectionMatrix[1][1] *= -1.0f;

        // View
        float pitch = glm::radians(transform.rotation.x);
        float yaw = glm::radians(transform.rotation.y);
        float roll = glm::radians(transform.rotation.z);

        glm::quat orientation = glm::quat(glm::vec3(-pitch, -yaw, -roll));
        m_viewMatrix = glm::translate(glm::mat4(1.0f), transform.position) * glm::toMat4(orientation);
        m_viewMatrix = glm::inverse(m_viewMatrix);

        m_viewProj = m_projectionMatrix * m_viewMatrix;
    }
}