#pragma once

#include "RHI/Interface/Types.h"

namespace psm
{
    class ICommandPool
    {
    public:
        ICommandPool() = default;
        virtual ~ICommandPool() = default;

        virtual void FreeCommandBuffers(const std::vector<CommandBufferPtr>& commandBuffers) = 0;
        virtual void* GetCommandPool() = 0;
    };
}