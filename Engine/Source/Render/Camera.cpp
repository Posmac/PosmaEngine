#include "Camera.h"

#include <iostream>

namespace psm
{
    Camera::Camera(float fieldOfViewDeg, 
        float aspect, 
        float nearPlane, 
        float farPlane)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDeg), aspect, nearPlane, farPlane);
        m_ProjectionMatrix[1][1] *= -1;//KOCTb|L' EBU4II
        m_ViewMatrix = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0), glm::vec3(0, 1, 0));
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

        m_InvProjectionMatrix = glm::inverse(m_ProjectionMatrix);
        m_InvViewMatrix = glm::inverse(m_ViewMatrix);
        m_InvViewProjectionMatrix = glm::inverse(m_ViewProjectionMatrix);
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

    glm::mat4& Camera::GetInvProjectionMatrix()
    {
        return m_InvProjectionMatrix;
    }

    glm::mat4& Camera::GetInvViewMatrix()
    {
        return m_InvViewMatrix;
    }

    glm::mat4& Camera::GetInvViewProjectionMatrix()
    {
        return m_InvViewProjectionMatrix;
    }

    glm::vec4& Camera::GetWorldPosition()
    {
        return m_InvViewMatrix[3];
    }

    glm::vec4& Camera::GetLocalPosition()
    {
        return m_ViewMatrix[3];
    }

    void Camera::TranslateWorld(const glm::vec4& offset)
    {
        m_InvViewMatrix[3] += offset;
    }

    void Camera::TranslateLocal(const glm::vec4& offset)
    {
        m_ViewMatrix[3] += offset;
    }

    void Camera::SetWorldPosition(const glm::vec4& worldPosition)
    {
        m_InvViewMatrix[3] = worldPosition;
    }

    void Camera::SetLocalPosition(const glm::vec4& localPosition)
    {
        m_ViewMatrix[3] = localPosition;
    }

    void Camera::RecalculateFromLocal()
    {
        m_InvViewMatrix = glm::inverse(m_ViewMatrix);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_InvViewProjectionMatrix = glm::inverse(m_ViewProjectionMatrix);
    }

    void Camera::RecalculateFromWorld()
    {
        m_ViewMatrix = glm::inverse(m_InvViewMatrix);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_InvViewProjectionMatrix = glm::inverse(m_ViewProjectionMatrix);
    }
}