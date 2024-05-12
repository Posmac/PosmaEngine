#pragma once

#include "RHI/Interface/Types.h"
#include "IObject.h"

namespace psm
{
    class ICommandPool : public IObject
    {
    public:
        ICommandPool() = default;
        virtual ~ICommandPool() = default;

        virtual void FreeCommandBuffers(const std::vector<CommandBufferPtr>& commandBuffers) = 0;
    };
}