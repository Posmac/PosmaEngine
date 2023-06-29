#include "Camera.h"

namespace psm
{
    Camera::Camera(float fieldOfViewDeg, 
        float aspect, 
        float nearPlane, 
        float farPlane)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDeg), aspect, nearPlane, farPlane);
        m_ViewMatrix = glm::mat4(1.0);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    glm::mat4& Camera::GetProjectionMatrix()
    {
        return m_ProjectionMatrix;
    }

    glm::mat4& Camera::GetViewMatrix()
    {
        return m_ViewMatrix;
    }

    glm::mat4& Camera::GetViewProjectionMatrix()
    {
        return m_ViewProjectionMatrix;
    }
}