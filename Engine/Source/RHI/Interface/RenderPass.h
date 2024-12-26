#pragma once

#include "RHI/Interface/Types.h"
#include "RHI/Configs/RenderPassConfig.h"
#include "IObject.h"

namespace psm
{
    class IRenderPass : public IObject
    {
    public:
        IRenderPass() = default;
        virtual ~IRenderPass() = default;

        virtual void BeginRenderPass(const SRenderPassBeginConfig& config) = 0;
        virtual void EndRenderPass(CommandBufferPtr commandBuffer) = 0;
        virtual uint32_t GetColorAttachmentsCount() const = 0;
        virtual bool HasDepthAttachment() const = 0;
    };
}