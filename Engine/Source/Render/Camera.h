#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace psm
{
    class Camera
    {
    public:
        //default perspective
        Camera() {};
        //perspective contructor
        Camera(float fieldOfViewDeg, 
            float aspect,
            float nearPlane, 
            float farPlane);
        glm::mat4& GetProjectionMatrix();
        glm::mat4& GetViewMatrix();
        glm::mat4& GetViewProjectionMatrix();

        glm::mat4& GetInvProjectionMatrix();
        glm::mat4& GetInvViewMatrix();
        glm::mat4& GetInvViewProjectionMatrix();

        glm::vec4& GetWorldPosition();
        glm::vec4& GetLocalPosition();

        void TranslateWorld(const glm::vec4& offset);
        void TranslateLocal(const glm::vec4& offset);
        void SetWorldPosition(const glm::vec4& worldPosition);
        void SetLocalPosition(const glm::vec4& localPosition);

        void RecalculateFromWorld();
        void RecalculateFromLocal();

    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::mat4 m_InvProjectionMatrix;
        glm::mat4 m_InvViewMatrix;
        glm::mat4 m_InvViewProjectionMatrix;
    };
}