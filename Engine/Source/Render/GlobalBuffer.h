#pragma once

#include "glm/glm.hpp"

namespace psm
{
    struct GlobalBuffer
    {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewProjectionMatrix;
        glm::vec2 ViewPortSize;
        float Time;
    };
}