#include "Vertex.h"

namespace psm
{
    bool Vertex::operator==(const Vertex& other) const
    {
        return Position == other.Position &&
            Normal == other.Normal &&
            TexCoord == other.TexCoord;
    }
}