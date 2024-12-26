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
        size_t operator()(const psm::Vertex& vertex) const
        {
            size_t h1 = hash<glm::vec4>()(vertex.Position);
            size_t h2 = hash<glm::vec4>()(vertex.Normal);
            size_t h3 = hash<glm::vec2>()(vertex.TexCoord);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}