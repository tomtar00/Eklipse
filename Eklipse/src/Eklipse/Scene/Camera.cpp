#include "precompiled.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Eklipse
{
    void Camera::OnUpdate(float aspectRatio)
    {
        glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
        projection[1][1] *= -1;

        float pitch = glm::radians(m_transform.rotation.x);
        float yaw = glm::radians(m_transform.rotation.y);
        float roll = glm::radians(m_transform.rotation.z);
        glm::vec3 cameraFront;
        cameraFront.x = cos(yaw) * cos(pitch);
        cameraFront.y = sin(pitch);
        cameraFront.z = sin(yaw) * cos(pitch);
        glm::vec3 up = { 0.0f, 1.0f, 0.0f };
        glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), roll, cameraFront);
        up = glm::mat3(roll_mat) * up;
        glm::mat4 view = glm::lookAt(m_transform.position, m_transform.position + glm::normalize(cameraFront), up);

        m_viewProj = projection * view;
    }
}