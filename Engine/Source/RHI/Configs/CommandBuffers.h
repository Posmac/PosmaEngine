#pragma once

#include <cstdint>

namespace psm
{
    enum class QueueType : uint8_t
    {
        GRAHPICS,
        COMPUTE,
        TRANSFER,
        GENERAL
    };

    struct CommandPoolConfig
    {
        uint32_t QueueFamilyIndex;
        QueueType QueueType;
    };

    struct CommandBufferConfig
    {
        uint32_t Size;
        bool IsBufferLevelPrimary;
    };
}