#pragma once

#include <optional>

#include "glm/glm.hpp"
#include "Vertex.h"

namespace psm
{
    struct MeshRange
    {
        uint32_t IndicesOffset;
        uint32_t VerticesOffset;
        uint32_t IndicesCount;
        uint32_t VerticesCount;
    };

    struct Mesh
    {
        MeshRange Range;

        glm::mat4 LocalMatrix;
        glm::mat4 InvLocalMatrix;
    };
}