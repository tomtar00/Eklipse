#include "precompiled.h"
#include "Camera.h"
#include "Components.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Eklipse
{
    void Camera::UpdateViewProjectionMatrix(Transform transform, float aspectRatio)
    {
        EK_PROFILE();

        // Projection
        glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
        projection[1][1] *= -1.0f;

        // View
        float pitch = glm::radians(transform.rotation.x);
        float yaw = glm::radians(transform.rotation.y);
        float roll = glm::radians(transform.rotation.z);

        glm::quat orientation = glm::quat(glm::vec3(-pitch, -yaw, -roll));
        glm::mat4 view = glm::translate(glm::mat4(1.0f), transform.position) * glm::toMat4(orientation);
        view = glm::inverse(view);

        m_viewProj = projection * view;
    }
}