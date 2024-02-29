#pragma once

#include "../VkCommon.h"
#include "../Configs/SurfaceConfig.h"
#include "Types.h"

namespace psm
{
    class ISurface
    {
    public:
        ISurface() = default;
        virtual ~ISurface() = default;
    };
}