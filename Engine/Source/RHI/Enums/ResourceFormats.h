#pragma once

#include <cstdint>

namespace psm
{
    enum class ESharingMode : uint32_t
    {
        EXCLUSIVE = 0,
        CONCURRENT = 1,
    };
}