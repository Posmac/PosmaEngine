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
    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;
    };
}