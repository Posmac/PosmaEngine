#pragma once

#include "../Configs/BufferConfig.h"
#include "../Interface/Types.h"
#include "../Memory/UntypedBuffer.h"

namespace psm
{
    class IDevice;

    class IBuffer
    {
    public:
        IBuffer() = default;
        virtual ~IBuffer() = default;
        virtual void Map(SBufferMapConfig& config) = 0;
        virtual void Unmap() = 0;
        virtual void Flush(SBufferFlushConfig& config) = 0;
        virtual void UpdateBuffer(const SUntypedBuffer& data) = 0;
        virtual void* GetMappedDataPtr() = 0;
    };
}