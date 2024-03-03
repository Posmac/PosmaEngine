#pragma once

#include "../Interface/Types.h"

namespace psm
{
    struct SSwapchainAquireNextImageConfig
    {
        uint64_t Timeout;
        SemaphorePtr Semaphore;
    };

    struct SSwapchainPresentConfig
    {
        uint32_t ImageIndex;
    };

    struct SSwapchainConfig
    {
        SurfacePtr Surface;
    };
}