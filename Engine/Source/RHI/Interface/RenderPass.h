#pragma once

#include "RHI/Interface/Types.h"
#include "RHI/Configs/RenderPassConfig.h"

namespace psm
{

    class IRenderPass
    {
    public:
        IRenderPass() = default;
        virtual ~IRenderPass() = default;

        virtual void BeginRenderPass(const SRenderPassBeginConfig& config) = 0;
        virtual void EndRenderPass(CommandBufferPtr commandBuffer) = 0;
        virtual void* GetNativeRawPtr() = 0;//TODO: abstract to IUnknown??
    };
}