#include "Camera.h"

#include <iostream>

static void LogVec4(const glm::vec4& v, char n)
{
    std::cout << v.x << " " << v.y << " " << v.z << " " << v.w << n;
}

namespace psm
{
    Camera::Camera()
    {

    }

    Camera::Camera(float fieldOfViewDeg,
                   float aspect,
                   float nearPlane,
                   float farPlane,
                   const glm::vec4& startingPosition,
                   const glm::quat& startingRotation)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDeg), aspect, nearPlane, farPlane);
        m_ProjectionMatrix[1][1] *= -1;//KOCTb|L' EBU4II
        m_InvProjectionMatrix = glm::inverse(m_ProjectionMatrix);

        m_CameraRotation = startingRotation;
        m_CameraPosition = startingPosition;
        m_InvViewMatrix = m_ViewMatrix = glm::mat4(1.0);
        RecalculateFromWorld();
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
        m_ViewMatrix[3] += offset[0] * GetRightWorld() + offset[1] * GetUpWorld() + offset[2] * GetForwardWorld();
    }

    void Camera::SetWorldPosition(const glm::vec4& worldPosition)
    {
        m_InvViewMatrix[3] = worldPosition;
    }

    void Camera::SetLocalPosition(const glm::vec4& localPosition)
    {
        m_ViewMatrix[3] = localPosition[0] * GetRightWorld() + localPosition[1] * GetUpWorld() + localPosition[2] * GetForwardWorld();;
    }

    void Camera::RecalculateFromWorld()
    {
        glm::mat3 basis = glm::mat3_cast(m_CameraRotation);

        glm::vec4 r1 = glm::vec4(basis[0], m_InvViewMatrix[3].x);
        glm::vec4 r2 = glm::vec4(basis[1], m_InvViewMatrix[3].y);
        glm::vec4 r3 = glm::vec4(basis[2], m_InvViewMatrix[3].z);

        m_InvViewMatrix =
        {
            r1.x, r2.x, r3.x, 0.0,
            r1.y, r2.y, r3.y, 0.0,
            r1.z, r2.z, r3.z, 0.0,
            r1.w, r2.w, r3.w, 1.0,
        };

        m_ViewMatrix = glm::inverse(m_InvViewMatrix);

        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_InvViewProjectionMatrix = glm::inverse(m_ViewProjectionMatrix);

        //LogCameraInfo();
    }

    void Camera::LogCameraInfo()
    {
        std::cout << "-----------------------------------------\n";
        std::cout << "Camera world pos: "; LogVec4(m_InvViewMatrix[3], '\n');
        std::cout << "Camera world Rig: "; LogVec4(GetRightWorld(), '\n');
        std::cout << "Camera world Up : "; LogVec4(GetUpWorld(), '\n');
        std::cout << "Camera world For: "; LogVec4(GetForwardWorld(), '\n');

        std::cout << "Camera local pos: "; LogVec4(m_ViewMatrix[3], '\n');
        std::cout << "Camera local Rig: "; LogVec4(GetRightLocal(), '\n');
        std::cout << "Camera local Up : "; LogVec4(GetUpLocal(), '\n');
        std::cout << "Camera local For: "; LogVec4(GetForwardLocal(), '\n');
    }

    void Camera::OnWindowResize(float fieldOfViewDeg,
                                float aspect,
                                float nearPlane,
                                float farPlane )
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(fieldOfViewDeg), aspect, nearPlane, farPlane);
        m_ProjectionMatrix[1][1] *= -1;//KOCTb|L' EBU4II

        m_InvProjectionMatrix = glm::inverse(m_ProjectionMatrix);

        //m_InvViewMatrix = m_ViewMatrix = glm::mat4(1.0);
        RecalculateFromWorld();
    }

    void Camera::RotateWorldEuler(const glm::vec3& offset)
    {
        glm::vec3 radiansOffset = glm::radians(offset);
        /*glm::quat rotation = glm::quat(radiansOffset);
        m_CameraRotation = rotation * m_CameraRotation;*/

        //multiplication order: Yaw -> Pitch -> Roll
        m_CameraRotation *= glm::angleAxis(radiansOffset.y, glm::vec3(0, 1, 0));
        m_CameraRotation *= glm::angleAxis(radiansOffset.x, glm::vec3(1, 0, 0));
        m_CameraRotation *= glm::angleAxis(radiansOffset.z, glm::vec3(0, 0, 1));

        glm::normalize(m_CameraRotation);
    }

    void Camera::RotateWorldEulerZConstrained(const glm::vec3& offset)
    {
        glm::vec3 radiansOffset = glm::radians(offset);
        m_CameraRotation *= glm::angleAxis(radiansOffset.x, glm::vec3(GetRightWorld()));
        m_CameraRotation *= glm::angleAxis(radiansOffset.y, glm::vec3(0, 1, 0));

        glm::normalize(m_CameraRotation);
    }

    void Camera::SetWorldRotationEuler(const glm::vec3& rotation)
    {
        glm::vec3 radiansOffset = glm::radians(rotation);
        //m_CameraRotation = glm::quat(radiansOffset);

        m_CameraRotation = glm::angleAxis(radiansOffset.y, glm::vec3(0, 1, 0));
        m_CameraRotation *= glm::angleAxis(radiansOffset.x, glm::vec3(1, 0, 0));
        m_CameraRotation *= glm::angleAxis(radiansOffset.z, glm::vec3(0, 0, 1));
    }

    glm::vec4& Camera::GetUpWorld()
    {
        return m_InvViewMatrix[1];
    }

    glm::vec4& Camera::GetUpLocal()
    {
        return m_ViewMatrix[1];
    }

    glm::vec4& Camera::GetForwardWorld()
    {
        return m_InvViewMatrix[2];
    }

    glm::vec4& Camera::GetForwardLocal()
    {
        return m_ViewMatrix[2];
    }

    glm::vec4& Camera::GetRightWorld()
    {
        return m_InvViewMatrix[0];
    }

    glm::vec4& Camera::GetRightLocal()
    {
        return m_ViewMatrix[0];
    }
}