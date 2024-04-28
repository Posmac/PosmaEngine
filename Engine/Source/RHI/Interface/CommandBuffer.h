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

        virtual void Reset() = 0;
        virtual void Begin(const SCommandBufferBeginConfig& config) = 0;
        virtual void End() = 0;
        virtual void* GetRawPointer() = 0;
    };
}