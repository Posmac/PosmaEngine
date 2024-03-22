#pragma once

#include "glm/glm.hpp"

namespace psm
{
    struct GlobalBuffer
    {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewProjectionMatrix;
        float Time;
    };
}