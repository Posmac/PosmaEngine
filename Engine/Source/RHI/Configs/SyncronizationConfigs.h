#pragma once

#include <cstdint>

namespace psm
{
    struct SFenceWaitConfig
    {
        bool WaitAll = true;
        float Timeout = FLT_MAX;
    };

    struct SFenceConfig
    {
        bool Signaled;
    };

    struct SSemaphoreConfig
    {
        bool Signaled;
    };
}
