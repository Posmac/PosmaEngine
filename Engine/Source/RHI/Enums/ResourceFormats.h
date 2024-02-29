#pragma once

#include <cstdint>

namespace psm
{
    enum class ESharingMode : uint8_t
    {
        EXCLUSIVE = 0,
        CONCURRENT = 1,
    };
}