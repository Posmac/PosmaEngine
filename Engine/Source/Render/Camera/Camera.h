#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace psm
{
    class Camera
    {
    public:
        //default perspective
        Camera();
        //perspective contructor
        Camera(float fieldOfViewDeg,
               float aspect,
               float nearPlane,
               float farPlane,
               const glm::vec4& startingPosition = glm::vec4(0, 0, 0, 1),
               const glm::quat& startingRotation = glm::quat(1, 0, 0, 0));
        glm::mat4& GetProjectionMatrix();
        glm::mat4& GetViewMatrix();
        glm::mat4& GetViewProjectionMatrix();

        glm::mat4& GetInvProjectionMatrix();
        glm::mat4& GetInvViewMatrix();
        glm::mat4& GetInvViewProjectionMatrix();

        //position
        glm::vec4& GetWorldPosition();
        glm::vec4& GetLocalPosition();

        void TranslateWorld(const glm::vec4& offset);
        void SetWorldPosition(const glm::vec4& worldPosition);
        void TranslateLocal(const glm::vec4& offset);
        void SetLocalPosition(const glm::vec4& localPosition);

        void RotateWorldEuler(const glm::vec3& offset);
        void SetWorldRotationEuler(const glm::vec3& rotation);
        void RotateWorldEulerZConstrained(const glm::vec3& offset);

        //basis vectors
        glm::vec4& GetUpWorld();
        glm::vec4& GetUpLocal();

        glm::vec4& GetForwardWorld();
        glm::vec4& GetForwardLocal();

        glm::vec4& GetRightWorld();
        glm::vec4& GetRightLocal();

        void RecalculateFromWorld();

        void LogCameraInfo();

        //events
        void OnWindowResize(float fieldOfViewDeg,
                            float aspect,
                            float nearPlane,
                            float farPlane);

    private:
        glm::vec4 m_CameraPosition; //in world space
        glm::quat m_CameraRotation; //in world space

        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::mat4 m_InvProjectionMatrix;
        glm::mat4 m_InvViewMatrix;
        glm::mat4 m_InvViewProjectionMatrix;
    };
}