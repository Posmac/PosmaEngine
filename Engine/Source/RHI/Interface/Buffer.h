#pragma once

#include "../Context/BufferContext.h"
#include "../RHISelector.h"

namespace psm
{
    class CDevice;

    class CBuffer
    {
    public:
        CBuffer() = default;
        virtual ~CBuffer() = default;
        virtual RHI_RESULT Init(CDevice* pDevice,
                                SResourceSize size,
                                EBufferUsage usage,
                                EMemoryProperties memoryProperties) = 0;
        virtual RHI_RESULT Map() = 0;
        virtual RHI_RESULT Unmap() = 0;
    };
}