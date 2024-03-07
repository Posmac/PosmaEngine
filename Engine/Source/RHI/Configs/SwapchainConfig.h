#pragma once

#include "../Interface/Types.h"

namespace psm
{
    struct SPresentConfig
    {
        void* Queue;
        uint32_t WaitSemaphoresCount;
        SemaphorePtr* pWaitSemaphores;
        uint32_t SwapchainsCount;
        SwapchainPtr* pSwapchains;
        uint32_t* ImageIndices;
    };

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