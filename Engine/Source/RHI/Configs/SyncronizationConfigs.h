#pragma once

#include <cstdint>

namespace psm
{
    struct SFenceConfig
    {
        bool Signaled;
    };

    struct SSemaphoreConfig
    {
        bool Signaled;
    };
}
