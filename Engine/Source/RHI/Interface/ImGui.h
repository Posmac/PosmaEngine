#pragma once

#include "RHI/Interface/Types.h"

namespace psm
{
    class IImGui
    {
    public:
        IImGui() = default;
        virtual ~IImGui() = default;
        
        virtual void PrepareNewFrame() = 0;
        virtual void Render(CommandBufferPtr commandBuffer) = 0;
    };
}