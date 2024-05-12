#pragma once

#include <cstdint>

namespace psm
{
    enum class EQueueType : uint32_t
    {
        GRAHPICS,
        COMPUTE,
        TRANSFER,
        GENERAL
    };

    enum class ECommandBufferUsage : uint32_t
    {
        NONE = 0x00000000,
        ONE_TIME_SUBMIT_BIT = 0x00000001,
        RENDER_PASS_CONTINUE_BIT = 0x00000002,
        SIMULTANEOUS_USE_BIT = 0x00000004,
    };
}