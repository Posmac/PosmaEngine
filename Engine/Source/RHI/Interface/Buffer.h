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
        virtual RESULT Init(DevicePtr& pDevice, const BufferConfig& config) = 0;
        virtual RESULT Map() = 0;
        virtual RESULT Unmap() = 0;
        virtual RESULT UpdateBuffer(const UntypedBuffer& data) = 0;
        virtual void* GetMappedDataPtr() = 0;
    };
}