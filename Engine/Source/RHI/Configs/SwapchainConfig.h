#pragma once

#include "../Interface/Types.h"

namespace psm
{
    struct SSwapchainPresentConfig
    {
        uint32_t ImageIndex;
    };

    struct SSwapchainConfig
    {
        SurfacePtr Surface;
    };
}