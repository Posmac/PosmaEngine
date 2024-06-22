#pragma once

#include "Foundation/Name.h"

#include <array>

namespace psm
{
    template <size_t Count>
    using NameArray = std::array<foundation::Name, Count>;

    namespace resourcenames
    {
        //shadow pass textures
        inline foundation::Name DirectionalLight {"Directional_Shadow_Pass_Texture"};
    }

    namespace samplernames
    {

    }

    //other names for PSO, Descriptor sets etc
}