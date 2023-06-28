#pragma once

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
        std::vector<Vertex> MeshVertices;
        std::vector<uint32_t> MeshIndices;

        MeshRange Range;
    };
}