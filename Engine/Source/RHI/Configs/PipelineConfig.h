#pragma once

#include <vector>

#include "RHI/Interface/Types.h"
#include "RHI/Enums/PipelineFormats.h"

namespace psm
{
    struct SVertexBufferBindConfig
    {
        uint32_t FirstSlot;
        uint32_t BindingCount;
        std::vector<uint64_t> Offsets;
        BufferPtr* Buffers;
    };

    struct SIndexBufferBindConfig
    {
        EIndexType Type;
        BufferPtr Buffer;
    };

    struct SPipelineConfig
    {

    };

    struct SPipelineBindConfig
    {

    };
}