#pragma once

#include <cstdint>

#include "RHI/Configs/BufferConfig.h"
#include "RHI/Interface/Types.h"
#include "RHI/Memory/UntypedBuffer.h"
#include "IObject.h"

namespace psm
{
    class IBuffer : public IObject
    {
    public:
        IBuffer() = default;
        virtual ~IBuffer() = default;
        virtual void Map(SBufferMapConfig& config) = 0;
        virtual void Unmap() = 0;
        virtual void Flush(SBufferFlushConfig& config) = 0;
        virtual void UpdateBuffer(const SUntypedBuffer& data) = 0;
        virtual uint64_t Size() = 0;
    };
}