#include "precompiled.h"
#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Eklipse
{
    void Camera::UpdateViewProjectionMatrix(float aspectRatio)
    {
        EK_PROFILE();

        // Projection
        glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
        projection[1][1] *= -1.0f;

        // View
        float pitch = glm::radians(m_transform.rotation.x);
        float yaw = glm::radians(m_transform.rotation.y);
        float roll = glm::radians(m_transform.rotation.z);

        glm::quat orientation = glm::quat(glm::vec3(-pitch, -yaw, -roll));
        glm::mat4 view = glm::translate(glm::mat4(1.0f), m_transform.position) * glm::toMat4(orientation);
        view = glm::inverse(view);

        m_viewProj = projection * view;
    }
}