#pragma once

#include <cstdint>

#include "RHI/Configs/CommandBuffers.h"

namespace psm
{
    class ICommandBuffer
    {
    public:
        ICommandBuffer() = default;
        virtual ~ICommandBuffer() = default;

        virtual void ResetAtIndex(uint32_t index) = 0;
        virtual void BeginAtIndex(const SCommandBufferBeginConfig& config) = 0;
        virtual void EndCommandBuffer(uint32_t index) = 0;
        virtual void* GetRawPointer() = 0;
    };
}