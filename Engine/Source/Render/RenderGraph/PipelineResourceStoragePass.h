#pragma once

#include "RHI/Interface/Buffer.h"

namespace psm
{
    namespace graph
    {
        struct PipelineResourceStoragePass
        {
            BufferPtr PassConstantBuffer;
            uint64_t PassConstantBufferMemoryOffset;
            uint64_t LastSetConstantBufferDataSize;
            bool IsAllowedToAdvanceConstantBufferOffset;
        };
    }
}