#pragma once

#include "RHI/Interface/Types.h"

namespace psm
{
    struct SPresentConfig
    {
        void* Queue;
        uint32_t WaitSemaphoresCount;
        SemaphorePtr* pWaitSemaphores;
        uint32_t ImageIndex;
    };

    struct SSwapchainAquireNextImageConfig
    {
        uint64_t Timeout;
        SemaphorePtr Semaphore;
    };

    struct SSwapchainConfig
    {
        SurfacePtr Surface;
    };
}