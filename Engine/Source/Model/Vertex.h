#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"

namespace psm
{
    struct Vertex
    {
        glm::vec4 Position;
        glm::vec4 Normal;
        glm::vec2 TexCoord;

        bool operator==(const Vertex& other) const;
    };
}

namespace std 
{
    template<> struct hash<psm::Vertex>
    {
        size_t operator()(psm::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.Position) ^
                (hash<glm::vec3>()(vertex.Normal) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.TexCoord) << 1);
        }
    };
}