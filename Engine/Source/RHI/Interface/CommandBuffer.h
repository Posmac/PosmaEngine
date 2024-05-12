#pragma once

#include <cstdint>

#include "RHI/Configs/CommandBuffers.h"
#include "IObject.h"

namespace psm
{
    class ICommandBuffer : public IObject
    {
    public:
        ICommandBuffer() = default;
        virtual ~ICommandBuffer() = default;

        virtual void Reset() = 0;
        virtual void Begin(const SCommandBufferBeginConfig& config) = 0;
        virtual void End() = 0;
    };
}