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
        virtual void SetViewport(CommandBufferPtr commandBuffer, float viewPortX, float viewPortY, float viewPortWidth, float viewPortHeight, float viewPortMinDepth, float viewPortMaxDepth) = 0;
        virtual void SetScissors(CommandBufferPtr commandBuffer, SResourceOffset2D scissorsOffet, SResourceExtent2D scissorsExtent) = 0;
        virtual void* GetNativeRawPtr() = 0;//TODO: abstract to IUnknown??
    };
}