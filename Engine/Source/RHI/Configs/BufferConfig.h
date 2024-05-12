#pragma once

#include "RHI/RHICommon.h"

#include "RHI/Enums/BufferFormats.h"

namespace psm
{
    struct SBufferConfig
    {
        SResourceSize Size;
        EBufferUsage Usage;
        EMemoryProperties MemoryProperties;
    };

    struct SBufferMapConfig
    {
        uint64_t Size;
        uint64_t Offset;
        void** pData;
        uint32_t Flags; //0 everythime for now
    };

    struct SBufferFlushConfig
    {
        uint64_t Size;
        uint64_t Offset;
    };
}