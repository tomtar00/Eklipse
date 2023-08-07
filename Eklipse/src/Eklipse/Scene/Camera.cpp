#include "precompiled.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Eklipse
{
    void Camera::OnUpdate(float aspectRatio)
    {
        {
            glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
            projection[1][1] *= -1;

            glm::vec3 cameraTarget = glm::vec3(0, 0, 0); 
            glm::mat4 view = glm::lookAt(m_transform.position, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

            m_viewProj = projection * view;
        }
    }
}
